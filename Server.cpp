/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 21:29:56 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/20 11:01:44 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//----------------------Constructors/Destructors-------------------------------

// Constructs + sets up all server attributes
Server::Server(int port, std::string password): _port(port), _password(password)
{
	this->setCmdMaps();
	this->_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (this->_fd < 0)
		throw (std::runtime_error("Server socket creation failed"));

	this->setSocket(htons(port), inet_addr(LOCALHOST));

	std::cout << GREY "Server has been properly set up" R << std::endl;
}

// Private copy constructor to force compilation error
Server::Server(Server const &src)
{
	throw (std::runtime_error("Copy constructor not allowed!"));
	(void)src;
}

// Destroys + frees memory and clears containers
Server::~Server()
{
	if (this->_fd > 0)
		close(this->_fd);
	this->_fds.clear();

	for (clients_iter it = this->_clients.begin(); it != this->_clients.end(); ++it)
		if (it->second)
			delete it->second;
	this->_clients.clear();
}

//-------------------------Operator overloads----------------------------------

// Private assignment operator to force compilation error
Server	&Server::operator=(Server const &src)
{
	throw (std::runtime_error("Assignment operator not allowed!"));

	(void)src;
	return (*this);
}

//---------------------------Getters/Setters-----------------------------------

// Returns server socket fd
int Server::getFd()const
{
	return (this->_fd);
}

// Returns port on which server is listening
int	Server::getPort()const
{
	return (this->_port);
}

// Returns client from _clients container that matches given fd
Client *Server::getClient(int fd)
{
	for (clients_iter it = this->_clients.begin(); it != this->_clients.end(); ++it)
	if (it->second->fd == fd)
		return (it->second);

	std::cerr << RED "Client with corresponding fd not found" R << std::endl;
	return (NULL);
}

//-----------------------------Init/setup--------------------------------------

// Sets socket options + defines socket address struct
void	Server::setSocket(in_port_t port, in_addr_t ip)
{
	int opt = 1;
	setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	this->_addr.sin_family = AF_INET;
	this->_addr.sin_addr.s_addr = ip;
	this->_addr.sin_port = port;

	this->_gen_addr = (struct sockaddr *)&this->_addr;
	this->_addrlen = sizeof(this->_addr);
}

// Launches server, listens to pollfd activity, adds new clients and treats events
void	Server::handleClient(size_t max_fds, int timeout)
{
	this->initServer(max_fds);

	while (true)
	{
		if (poll(this->_fds.data(), this->_fds.size(), timeout) < 0)
			throw (std::runtime_error("Poll failed"));

		if (this->_fds[0].revents & POLLIN)
		{
			if (this->_fds.size() >= max_fds -1)
				throw (std::runtime_error("All client slots are taken!"));

			this->addSocket(true);
		}

		this->treatRevent();
	}
}

// Binds socket fd with local address, prepares to listen to connections, add server socket to pollfd vector
void	Server::initServer(int max_fds)
{
	if (bind(this->_fd, this->_gen_addr, this->_addrlen) < 0)
		throw (std::runtime_error("Server binding failed"));

	if (listen(this->_fd, max_fds) < 0)
		throw(std::runtime_error("Server fails to listen"));

	this->addSocket(false);
}

// Adds new socket to pollfd vector + adds it to _client map & sets to non-blocking if is a client
void	Server::addSocket(bool isclient)
{
	struct pollfd newpoll = {};
	newpoll.events = POLLIN;

	if (!isclient)
		newpoll.fd = this->_fd;
	else
	{
		newpoll.fd = accept(this->_fd, this->_gen_addr, &this->_addrlen);
		if (newpoll.fd < 0)
			throw (std::runtime_error("Server failed to accept client connection"));

		fcntl(newpoll.fd, F_SETFL, O_NONBLOCK);

		Client *newclient = new Client(newpoll.fd);
		newclient->state = AT_DOOR;
		this->_clients[newclient->nickname]= newclient;

		this->welcomeScreen(*newclient);
	}

	this->_fds.push_back(newpoll);
}

//----------------------------Poll/revents-------------------------------------

// Loops around all pollfds for revent activity - if found, sends to relevant event-managing method
void	Server::treatRevent()
{
	if (this->_fds.size() <= 1)
		return ;

	pollfd_iter it = this->_fds.begin() +1;

	for (; it != this->_fds.end(); ++it)
	{
		Client *client = this->getClient(it->fd);
		if (!client)
			continue ;
		switch (it->revents)
		{
			case POLLHUP:
				this->pollHup(*client); break;
			case POLLIN:
				this->pollIn(*client); break;
			case POLLERR:
				this->pollErr(*client); break;
			case POLLNVAL:
				this->pollNVal(*client); break;
			default:
				break;
		}
		if (it->fd == REMOVAL)
			this->_fds.erase(it--);
	}
}

// POLLUP = client left
// Sends to client removal function
void	Server::pollHup(Client &client)
{
	this->removeClient(client);
}

// POLLIN = message sent by client
// Retrieves message, splits it into vector, sends it to command managers
void Server::pollIn(Client &client)
{
	std::string msg = getMsg(client);
	if (msg.empty())
		return;

	std::vector<std::string> split_msg = this->splitMsg(msg);
	Channel *channel = findChannel(split_msg);

	std::string output;

	if (this->_authcmds.find(split_msg[0]) != this->_authcmds.end())
		output = (this->*_authcmds[split_msg[0]])(&client, channel, split_msg);
	else if (channel && this->_chancmds.find(split_msg[0]) != this->_chancmds.end())
		output = (channel->*_chancmds[split_msg[0]])(client, split_msg);
	else
		output = RED "Invalid - command not recognised" R;

	this->broadcast(client, channel, output);
}

// POLLERR = error occurred with fd
// Depending on error code: ignores, reopen socket, or removes client
void	Server::pollErr(Client &client)
{
	std::string error = strerror(errno);
	send(client.fd, error.c_str(), error.length(), 0);
	std::cerr << RED "Error occurred with client " << client.nickname << ":" + error << R << std::endl;

	if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
	{
		std::cout << "Retrying ..." << std::endl;
		return ;
	}
	else if (errno == ETIMEDOUT)
	{
		std::cout << "Reopening attempt ..." << std::endl;
		close(client.fd);

		client.fd = socket(AF_INET, SOCK_STREAM, 0);
		if (client.fd >= 0)
		{
			fcntl(client.fd,  F_SETFL, O_NONBLOCK);
			std::cout << PURPLE "Reconnection successful" R << std::endl;
			return ;
		}
	}

	std::cerr << RED "Unrecoverable - closing socket" R << std::endl;
	this->removeClient(client);
}

// POLLNVAL = file descriptor issue
// Shows error message + sends to client removal
void	Server::pollNVal(Client &client)
{
	std::cerr << RED "File descriptor " << client.fd << " is invalid" R << std::endl;
	std::cerr << RED "Unrecoverable - closing socket" R << std::endl;
	this->removeClient(client);
}

//------------------------------Commands---------------------------------------
// If client state, command format, password are correct -> sets client to next state
std::string Server::passCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{
	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->state > AT_DOOR)
		return (PURPLE "You are already logged into the server" R);

	if (split_msg.size() != 2)
		return (RED "JOIN: invalid command format\n" GREY "Expected: JOIN <password>" R);

	if (split_msg[1] != this->_password)
		return (RED "Invalid password - access denied" R);

	std::cout << PURPLE << client->nickname << R " has been granted access" << std::endl;
	client->state = PASS_OK;

	return (GREY "Password is correct - access granted!\nPlease proceed with NICK" R);
}

// If client state & command format are correct -> sets existing client as active - or sets non-existing client to next state
std::string Server::nickCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{
	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->state < PASS_OK)
		return (RED "Error - Please enter the server using PASS <password>" R);
	else if (client->state > PASS_OK)
		return (PURPLE "Your nickname has already been set" R);

	if (split_msg.size() != 2)
		return (RED "JOIN: invalid command format\n" GREY "Expected: NICK <nickname>" R);

	if (this->_clients.find(split_msg[1]) == this->_clients.end())
	{
		std::cout << PURPLE << client->nickname << R " has set nickname to " GREY << split_msg[1] << R << std::endl;
		client->state = NICK_OK;
		client->nickname = split_msg[1];
		return (GREY "Nickname created - Please proceed with USER" R);
	}

	this->_clients.erase(client->nickname);
	delete client;

	client = this->_clients[split_msg[1]];
	client->state = ACTIVE;
	std::cout << PURPLE << client->nickname << R " has logged in" << std::endl;

	return (PURPLE "Welcome back, " + client->nickname + R);
}

// If client state & command format are correct -> updates username and realname - finalizes non-existing client registration
std::string Server::userCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{
	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->state <= AT_DOOR)
		return (RED "Error - Please enter the server using PASS" R);
	else if (client->state == PASS_OK)
		return (RED "Error - Please create a nickname using NICK" R);

	if (split_msg.size() < 5 || split_msg[4][0] != ':' || split_msg[4].size() <= 1)
		return (RED "JOIN: invalid command format\n" GREY "Expected: USER <username> <hostname> <servername> :<realname>" R);

	client->username = split_msg[1];
	client->realname = &split_msg[4][1];
	for (size_t i = 5; i < split_msg.size(); i++)
	{
		client->realname += split_msg[i];
		if (i != split_msg.size() -1)
			client->realname += " ";
	}

	if (this->_clients.find(client->nickname) != this->_clients.end())
	{
		std::cout << PURPLE << client->nickname << R " has changed its user data to:\n"
		<< GREY " > username: " R << client->username << GREY "	-	realname: " R << client->realname << std::endl;

		return (PURPLE "Your user data has been correctly updated" R);
	}

	client->state = ACTIVE;
	std::cout << PURPLE << client->nickname << R " has completed registration\n"
	<< GREY " > username: " R << client->username << GREY " - realname: " R << client->realname << std::endl;

	return (PURPLE "Welcome, " + client->nickname + R);
}

std::string Server::joinCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{

}

//--------------------------------Utils----------------------------------------

// Creates maps linking command to an function
void	Server::setCmdMaps()
{
	this->_authcmds["PASS"] = &Server::passCmd;
	this->_authcmds["NICK"] = &Server::nickCmd;
	this->_authcmds["USER"] = &Server::userCmd;
	this->_authcmds["JOIN"] = &Server::joinCmd;

	this->_chancmds["MODE"] = &Channel::modeCmd;
	this->_chancmds["TOPIC"] = &Channel::topicCmd;
	this->_chancmds["INVITE"] = &Channel::inviteCmd;
	this->_chancmds["PRIVMSG"] = &Channel::privmsgCmd;
	this->_chancmds["KICK"] = &Channel::kickCmd;
}

// Displays welcome screen to user + notifies server
void	Server::welcomeScreen(Client &client)
{
	std::cout << PURPLE << client.nickname << R " is at the door" << std::endl;

	std::string msg = PURPLE WELCOME GREY INSTRUCTIONS R;
	if (send(client.fd, msg.c_str(), msg.length(), 0) < 0)
		throw std::runtime_error("Failed to send welcome message");
}

// Retrieves message from user + perform checks - removes client if errors
std::string Server::getMsg(Client &client)
{
	char buff[BUFFSIZE];
	memset(buff, 0, BUFFSIZE);

	int bytes_read = recv(client.fd, buff, BUFFSIZE, 0);

	if (bytes_read <= 0)
	{
		if (bytes_read < 0)
			broadcast(client, NULL, strerror(errno));
		this->removeClient(client);
		buff[0] = 0;
	}

	return (std::string(buff));
}

// Splits user command into vector string items
std::vector<std::string> Server::splitMsg(std::string &msg)
{
	std::vector<std::string> split_msg;
	size_t pos = 0;

	while (pos <= msg.size())
	{
		pos = msg.find_first_of(" \t\0");
		split_msg.push_back(msg.substr(0, pos));
		if (pos == msg.size())
			break;
		msg = msg.substr(pos +1, msg.size());
	}

	if (split_msg.size() < 2)
		split_msg.push_back("INVALID");

	return (split_msg);
}

Channel	*Server::findChannel(std::vector<std::string> &split_msg)
{
	Channel *channel = NULL;
	std::string channel_name;

	if (split_msg.size() >= 2 && split_msg[1][0] == '#')
		channel_name = split_msg[1][0];
	else if (split_msg.size() >= 3 && split_msg[2][0] == '#' && split_msg[0] == "INVITE")
		channel_name = split_msg[2][0];

	if (this->_channels.find(channel_name) != this->_channels.end())
		channel = this->_channels[channel_name];

	return (channel);
}

// Broadcasts message to server, client, & channel if applicable
void	Server::broadcast(Client &client, Channel *channel, std::string const &msg)
{
	std::string output = PURPLE + client.nickname + ": " R + msg;
	std::cout << output << std::endl;

	if (!channel)
	{
		if (send(client.fd, output.c_str(), output.length(), 0) < 0)
			throw (std::runtime_error("Failed to send to socket " + client.fd));
		return;
	}

	std::vector<Client *>::iterator it = channel->members.begin();
	for (; it != channel->members.end(); ++it)
		if (&client != *it && (*it)->state == ACTIVE)
			if (send((*it)->fd, output.c_str(), output.length(), 0) < 0)
				throw (std::runtime_error("Failed to send to " + (*it)->nickname));
}

// Removes client from pollfd, sets client as OFFLINE, closes fd, broadcast departure
void	Server::removeClient(Client &client)
{
	for (pollfd_iter it = this->_fds.begin(); it != this->_fds.end(); ++it)
		if (client.fd == it->fd)
			it->fd = REMOVAL;

	for (channels_iter it = this->_channels.begin(); it != this->_channels.end(); ++it)
		if (std::find(it->second->members.begin(), it->second->members.end(), &client) != it->second->members.end())
			this->broadcast(client, it->second, "has left");

	close(client.fd);
	client.state = OFFLINE;
}


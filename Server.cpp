/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 21:29:56 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/19 14:50:04 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, std::string password): _port(port), _password(password)
{
	this->cmdMap.insert(std::make_pair("PASS", PASS));
	this->cmdMap.insert(std::make_pair("NICK", NICK));
	this->cmdMap.insert(std::make_pair("USER", USER));
	this->cmdMap.insert(std::make_pair("JOIN", JOIN));
	this->cmdMap.insert(std::make_pair("MODE", MODE));
	this->cmdMap.insert(std::make_pair("TOPIC", TOPIC));
	this->cmdMap.insert(std::make_pair("INVITE", INVITE));
	this->cmdMap.insert(std::make_pair("PRIVMSG", PRIVMSG));
	this->cmdMap.insert(std::make_pair("KICK", KICK));

	/* this->cmdMap = {
		{"PASS", PASS},
		{"NICK", NICK},
		{"USER", USER},
		{"JOIN", JOIN},
		{"MODE", MODE},
		{"TOPIC", TOPIC},
		{"INVITE", INVITE},
		{"PRIVMSG", PRIVMSG},
		{"KICK", KICK}
	}; */

	this->_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (this->_fd < 0)
		throw (std::runtime_error("Server socket creation failed"));

	this->setSocket(htons(port), inet_addr(LOCALHOST));

	std::cout << GREY "Server has been properly set up" R << std::endl;
}

Server::~Server()
{
	if (this->_fd > 0)
		close(this->_fd);
	this->_fds.clear();

	for (online_iter it = this->_online.begin(); it != this->_online.end(); ++it)
	{
		if (it->first > 0)
			close(it->first);
		if (it->second)
			delete it->second;
	}
	this->_online.clear();

	for (clients_iter it = this->_clients.begin(); it != this->_clients.end(); ++it)
		if (it->second)
			delete it->second;
}

int Server::getFd()const
{
	return (this->_fd);
}

int	Server::getPort()const
{
	return (this->_port);
}

Client &Server::getClient(int fd)
{
	if (this->_online.find(fd) == this->_online.end())
		throw (std::runtime_error("Client with corresponding fd not found"));

	return (*this->_online[fd]);
}

void	Server::initServer(int max_fds)
{
	if (bind(this->_fd, this->_gen_addr, this->_addrlen) < 0)
		throw (std::runtime_error("Server binding failed"));

	if (listen(this->_fd, max_fds) < 0)
		throw(std::runtime_error("Server fails to listen"));

	this->addSocket(false);
}

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

void	Server::treatRevent()
{
	if (this->_fds.size() <= 1)
		return ;

	pollfd_iter it = this->_fds.begin() +1;

	for (; it != this->_fds.end(); ++it)
	{
		Client *client = this->_online[it->fd];
		switch (it->revents)
		{
			case POLLHUP:
				this->pollHup(it); break;
			case POLLIN:
				this->pollIn(*client); break;
			case POLLERR:
				this->pollErr(it); break;
			case POLLNVAL:
				this->pollNVal(); break;
			default:
				break;
		}
		if (it->fd == REMOVAL)
			this->_fds.erase(it--);
	}
}

void	Server::removeClient(Client *client)
{
	if (!client)
		return;

	if (this->_clients.find(client->nickname) != this->_clients.end())
		this->_clients[client->nickname]->state = OFFLINE;

	for (pollfd_iter it = this->_fds.begin(); it != this->_fds.end(); ++it)
		if (client->fd == it->fd)
			it->fd = REMOVAL;

	online_iter online_client = this->_online.find(client->fd);
	close(online_client->first);
	this->_online.erase(online_client);

	std::map<std::string, Channel *>::iterator it = this->_channels.begin();
	for (; it != this->_channels.end(); ++it)
		if (std::find(it->second->members.begin(), it->second->members.end(), client) != it->second->members.end())
			this->broadcast(*client, it->second, "has left");
	client->state = OFFLINE;
}

bool	Server::hasClient()
{
	return (!this->_online.empty());
}

Server::Server(Server const &src)
{
	throw (std::runtime_error("Copy constructor not allowed!"));
	(void)src;
}

Server	&Server::operator=(Server const &src)
{
	throw (std::runtime_error("Assignment operator not allowed!"));

	(void)src;
	return (*this);
}

void	Server::setSocket(in_port_t port, in_addr_t ip)
{
	//Set socket options
	int opt = 1;
	setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	//Define socket address
	this->_addr.sin_family = AF_INET;
	this->_addr.sin_addr.s_addr = ip;
	this->_addr.sin_port = port;

	this->_gen_addr = (struct sockaddr *)&this->_addr;
	this->_addrlen = sizeof(this->_addr);
}

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
		this->_online[newpoll.fd] = newclient;

		this->welcomeScreen(*newclient);
	}

	this->_fds.push_back(newpoll);
}

void	Server::welcomeScreen(Client &client)
{
	std::cout << PURPLE << client.nickname << R " is at the door" << std::endl;

	std::string msg = PURPLE WELCOME GREY INSTRUCTIONS R;
	if (send(client.fd, msg.c_str(), msg.length(), 0) < 0)
		throw std::runtime_error("Failed to send welcome message");
}

void	Server::pollIn(Client &client)
{
	std::string msg = getMsg(client);
	if (msg.empty())
		return ;

	std::vector<std::string> split_msg = this->splitMsg(msg);
	CmdsEnum cmd = this->cmdMap.find(split_msg[0])->second;

	std::string output;
	if (cmd < 4)
		this->authCmds(split_msg, cmd, client);
	else if (cmd < 10)
		this->channelCmds(split_msg, cmd, client);
}

void	Server::pollErr(pollfd_iter &it)
{
	std::string error = strerror(errno);
	send(it->fd, error.c_str(), error.length(), 0);

	throw (std::runtime_error("Error occurred in client socket :" + error));

	/* to add:
		-> try to recover connection - unsure how
		-> if not recoverable, cleanup resources*/
}

void	Server::pollHup(pollfd_iter &it)
{
	this->removeClient(this->_online[it->fd]);
	/* to add:
		-> remove client form active conversations*/
}

void	Server::pollNVal()
{
	std::cout << "POLLNVAL triggered" << std::endl;
	throw (std::runtime_error("Invalid socket descriptor"));
}

std::string Server::getMsg(Client &client)
{
	char buff[BUFFSIZE];
	memset(buff, 0, BUFFSIZE);

	int bytes_read = recv(client.fd, buff, BUFFSIZE, 0);

	if (bytes_read <= 0)
	{
		if (bytes_read < 0)
			broadcast(client, NULL, strerror(errno));
		this->removeClient(&client);
		buff[0] = 0;
	}

	return (std::string(buff));
}

std::vector<std::string> Server::splitMsg(std::string msg)
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

void	Server::authCmds(std::vector<std::string> &split_msg, CmdsEnum cmd, Client &client)
{
	std::string output;

	switch (cmd)
	{
		case INVALID:
			output = RED "Invalid - command not recognised" R; break;
		case PASS:
			output = this->passCmd(client, split_msg); break;
		case NICK:
			output = this->nickCmd(client, split_msg); break;
		case USER:
			output = this->userCmd(client, split_msg); break;
		default:
			break;
	}

	if (send(client.fd, output.c_str(), output.length(), 0) < 0)
		throw (std::runtime_error("Failed to send to socket " + client.fd));
}

void	Server::channelCmds(std::vector<std::string> &split_msg, CmdsEnum cmd, Client &client)
{
	std::string output;
	Channel *channel;


	if (split_msg[1][0] == '#')
	{
		if (this->_channels.find(split_msg[1]) != this->_channels.end())
			channel = this->_channels[split_msg[1]];
	}
	else if (cmd == INVITE && split_msg.size() > 2 && split_msg[2][0] == '#')
	{
		if (this->_channels.find(split_msg[1]) != this->_channels.end())
			channel = this->_channels[split_msg[1]];
	}
	else if (cmd != JOIN)
		this->broadcast(client, NULL, RED "Invalid - channel not found" R);

	switch (cmd)
	{
		case JOIN:
			if (!channel)
			{
				this->_channels[split_msg[1]] = new Channel(split_msg[1]);
				output = PURPLE "Channel created\n" R;
			}
			output += channel->joinCmd(client, split_msg); break;
		case MODE:
			output = channel->modeCmd(client, split_msg); break;
		case TOPIC:
			output = channel->topicCmd(client, split_msg); break;
		case INVITE:
			output = channel->inviteCmd(client, split_msg); break;
		case PRIVMSG:
			output = channel->privmsgCmd(client, split_msg); break;
		case KICK:
			output = channel->kickCmd(client, split_msg); break;
		default:
			break;
	}

	this->broadcast(client, channel, output);
}

std::string Server::passCmd(Client &client, std::vector<std::string> &split_msg)
{
	if (client.state > AT_DOOR)
		return (PURPLE "You are already logged into the server" R);

	if (split_msg.size() != 2)
		return (RED "JOIN: invalid command format\n" GREY "Expected: JOIN <password>" R);

	if (split_msg[1] != this->_password)
		return (RED "Invalid password - access denied" R);

	std::cout << PURPLE << client.nickname << R " has been granted access" << std::endl;
	client.state = PASS_OK;

	return (GREY "Password is correct - access granted!\nPlease proceed with NICK" R);
}

std::string Server::nickCmd(Client &client, std::vector<std::string> &split_msg)
{
	if (client.state < PASS_OK)
		return (RED "Error - Please enter the server using PASS <password>" R);
	else if (client.state > PASS_OK)
		return (PURPLE "Your nickname has already been set" R);

	if (split_msg.size() != 2)
		return (RED "JOIN: invalid command format\n" GREY "Expected: NICK <nickname>" R);

	if (this->_clients.find(split_msg[1]) == this->_clients.end())
	{
		std::cout << PURPLE << client.nickname << R " has set nickname to " GREY << split_msg[1] << R << std::endl;
		client.state = NICK_OK;
		client.nickname = split_msg[1];
		return (GREY "Nickname created - Please proceed with USER" R);
	}

	client = *this->_clients[split_msg[1]];
	client.state = ACTIVE;
	std::cout << PURPLE << client.nickname << R " has logged in" << std::endl;

	return (PURPLE "Welcome back, " + client.nickname + R);
}

std::string Server::userCmd(Client &client, std::vector<std::string> &split_msg)
{
	if (client.state <= AT_DOOR)
		return (RED "Error - Please enter the server using PASS" R);
	else if (client.state == PASS_OK)
		return (RED "Error - Please create a nickname using NICK" R);

	if (split_msg.size() < 5 || split_msg[4][0] != ':' || split_msg[4].size() <= 1)
		return (RED "JOIN: invalid command format\n" GREY "Expected: USER <username> <hostname> <servername> :<realname>" R);

	client.username = split_msg[1];
	client.realname = &split_msg[4][1];
	for (size_t i = 5; i < split_msg.size(); i++)
	{
		client.realname += split_msg[i];
		if (i != split_msg.size() -1)
			client.realname += " ";
	}

	if (this->_clients.find(client.nickname) != this->_clients.end())
	{
		std::cout << PURPLE << client.nickname << R " has changed its user data to:\n"
		<< GREY " > username: " R << client.username << GREY "	-	realname: " R << client.realname << std::endl;

		return (PURPLE "Your user data has been correctly updated" R);
	}

	client.state = ACTIVE;
	this->_clients[client.nickname] = new Client(client);

	std::cout << PURPLE << client.nickname << R " has completed registration\n"
	<< GREY " > username: " R << client.username << GREY " - realname: " R << client.realname << std::endl;

	return (PURPLE "Welcome, " + client.nickname + R);
}

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

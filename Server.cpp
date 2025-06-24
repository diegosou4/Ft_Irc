/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 21:29:56 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/19 17:24:57 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, std::string password): _port(port), _password(password)
{
	this->cmdMap.insert(std::make_pair("CAP", CAP));
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

	for (clients_iter it = this->_clients.begin(); it != this->_clients.end(); ++it)
		if (it->second)
			delete it->second;

	this->_clients.clear();
}

int Server::getFd()const
{
	return (this->_fd);
}

int	Server::getPort()const
{
	return (this->_port);
}

Client *Server::getClient(int fd)
{
	for (clients_iter it = this->_clients.begin(); it != this->_clients.end(); ++it)
		if (it->second->getClientFd() == fd)
			return (it->second);

	std::cout << this->_fds.size() << " fds in the vector" << std::endl;
	std::cout << this->_fds[0].fd << " is the server fd" << std::endl;

	std::cerr << RED "Client with corresponding fd not found" << fd << R << std::endl;
	return (NULL);
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

void Server::treatRevent(void)
{
	std::vector<pollfd>::iterator it = this->_fds.begin();
	while (it != this->_fds.end())
	{
		if (it->fd == this->_fd)
		{
			++it;
			continue;
		}
		Client *client = this->getClient(it->fd);

		if (!client)
		{
			++it;
			continue;
		}

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
			it = this->_fds.erase(it);
		else
			++it;
	}
}



void	Server::removeClient(Client &client)
{
	for (pollfd_iter it = this->_fds.begin(); it != this->_fds.end(); ++it)
		if (client.getClientFd() == it->fd)
			it->fd = REMOVAL;

	for (channels_iter it = this->_channels.begin(); it != this->_channels.end(); ++it)
		if (std::find(it->second->members.begin(), it->second->members.end(), &client) != it->second->members.end())
			this->broadcast(client, it->second, "has left");

	close(client.getClientFd());
	client.setState(OFFLINE);
}

bool	Server::hasClient()
{
	for (clients_iter it = this->_clients.begin(); it != this->_clients.end(); ++it)
		if (it->second->getState() == ACTIVE)
			return (true);

	return (false);
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
		newclient->setState(AT_DOOR);
		this->_clients[newclient->getNickname()] = newclient;

		this->welcomeScreen(*newclient);
	}

	this->_fds.push_back(newpoll);
}

void	Server::welcomeScreen(Client &client)
{
	
	std::cout << PURPLE << client.getNickname() << R " is at the door" << std::endl;

	std::string msg = PURPLE WELCOME GREY INSTRUCTIONS R;
	if (send(client.getClientFd(), msg.c_str(), msg.length(), 0) < 0)
		throw std::runtime_error("Failed to send welcome message");
}

void	Server::pollIn(Client &client)
{
	std::string msg = getMsg(client);
	// Debugging output
	std::cout << "Raw message received: [" << msg << "]" << std::endl;

	if (msg.empty())
		return ;
	
	std::vector<std::string> split_msg = this->splitMsg(msg);

	std::map<std::string, CmdsEnum>::iterator it = this->cmdMap.find(split_msg[0]);
	if (it == this->cmdMap.end())
		throw std::runtime_error("Command not found: " +  msg);

	CmdsEnum cmd = it->second;


	std::string output;
	if (cmd < 4)
		this->authCmds(split_msg, cmd, client);
	else if (cmd < 10)
		this->channelCmds(split_msg, cmd, client);
}

void	Server::pollErr(Client &client)
{
	std::string error = strerror(errno);
	send(client.getClientFd(), error.c_str(), error.length(), 0);
	std::cerr << RED "Error occurred with client " << client.getNickname() << ":" + error << R << std::endl;

	if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
	{
		std::cout << "Retrying ..." << std::endl;
		return ;
	}
	else if (errno == ETIMEDOUT)
	{
		std::cout << "Reopening attempt ..." << std::endl;
		close(client.getClientFd());

		client.setClientFd(socket(AF_INET, SOCK_STREAM, 0));
		if (client.getClientFd() >= 0)
		{
			fcntl(client.getClientFd(),  F_SETFL, O_NONBLOCK);
			std::cout << PURPLE "Reconnection successful" R << std::endl;
			return ;
		}
	}

	std::cerr << RED "Unrecoverable - closing socket" R << std::endl;
	this->removeClient(client);
}

void	Server::pollHup(Client &client)
{
	this->removeClient(client);
}

void	Server::pollNVal(Client &client)
{
	std::cerr << RED "File descriptor " << client.getClientFd() << " is invalid" R << std::endl;
	std::cerr << RED "Unrecoverable - closing socket" R << std::endl;
	this->removeClient(client);
}

std::string Server::getMsg(Client &client)
{
    char buff[BUFFSIZE];
    memset(buff, 0, BUFFSIZE);

    int bytes_read = recv(client.getClientFd(), buff, BUFFSIZE, 0);

    if (bytes_read <= 0)
    {
        if (bytes_read < 0)
            broadcast(client, NULL, strerror(errno));
        this->removeClient(client);
        return std::string();
    }

    return std::string(buff, bytes_read);
}


std::vector<std::string> Server::splitMsg(std::string msg)
{
	std::vector<std::string> split_msg;
	size_t colon_pos = msg.find(':');

	std::string head = (colon_pos != std::string::npos) ? msg.substr(0, colon_pos) : msg;
	std::string tail = (colon_pos != std::string::npos) ? msg.substr(colon_pos) : "";

	std::istringstream iss(head);
	std::string token;
	while (iss >> token)
		split_msg.push_back(token);

	if (!tail.empty())
		split_msg.push_back(tail);

	return split_msg;
}


void	Server::authCmds(std::vector<std::string> &split_msg, CmdsEnum cmd, Client &client)
{
	std::string output;

	switch (cmd)
	{
		case CAP: 
			output = this->capCmd(client, split_msg); break;
		case INVALID:
			output = RED "Invalid - command not recognised" R; break;
		case PASS:
			output = this->passCmd(client, split_msg); break;
		case NICK:
			output = this->nickCmd(&client, split_msg); break;
		case USER:
			output = this->userCmd(client, split_msg); break;
		default:
			break;
	}

	if (send(client.getClientFd(), output.c_str(), output.length(), 0) < 0)
		throw (std::runtime_error("Failed to send to socket " + client.getClientFd()));
}

void	Server::channelCmds(std::vector<std::string> &split_msg, CmdsEnum cmd, Client &client)
{
	std::string output;
	Channel *channel;

	std::cout << "Channel command received: " << cmd << std::endl;
		for(std::vector<std::string>::iterator messagePart = split_msg.begin(); messagePart != split_msg.end(); ++messagePart)
		{
			std::cout << "Split message part: " << *messagePart << std::endl;
		}
	

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

	if (client.getState() > AT_DOOR)
		return (PURPLE "You are already logged into the server" R);

	if (split_msg.size() != 2 || split_msg[0] == "JOIN")
		return (RED "JOIN: invalid command format\n" GREY "Expected: JOIN <password>" R);

	if (split_msg[1] != this->_password)
		return (RED "Invalid password - access denied" R);

	std::cout << PURPLE << client.getNickname() << R " has been granted access" << std::endl;
	client.setState(PASS_OK);

	return (GREY "Password is correct - access granted!\nPlease proceed with NICK" R);
}

std::string Server::nickCmd(Client *client, std::vector<std::string> &split_msg)
{
	if (client->getState() < PASS_OK)
		return (RED "Error - Please enter the server using PASS <password>" R);
	else if (client->getState() > PASS_OK)
		return (PURPLE "Your nickname has already been set" R);

	if (split_msg.size() != 2)
		return (RED "JOIN: invalid command format\n" GREY "Expected: NICK <nickname>" R);

	if (this->_clients.find(split_msg[1]) == this->_clients.end())
	{
		std::cout << PURPLE << client->getNickname() << R " has set nickname to " GREY << split_msg[1] << R << std::endl;
		client->setState(NICK_OK);
		client->setNickname(split_msg[1]);
		return (GREY "Nickname created - Please proceed with USER" R);
	}

	this->_clients.erase(client->getNickname());
	delete client;

	client = this->_clients[split_msg[1]];
	client->setState(ACTIVE);
	std::cout << PURPLE << client->getNickname() << R " has logged in" << std::endl;

	return (PURPLE "Welcome back, " + client->getNickname() + R);
}

std::string Server::userCmd(Client &client, std::vector<std::string> &split_msg)
{
	if (client.getState() <= AT_DOOR)
		return (RED "Error - Please enter the server using PASS" R);
	else if (client.getState() == PASS_OK)
		return (RED "Error - Please create a nickname using NICK" R);

	if (split_msg.size() < 5 || split_msg[4][0] != ':' || split_msg[4].size() <= 1 )
		return (RED "JOIN: invalid command format\n" GREY "Expected: USER <username> <hostname> <servername> :<realname>" R);

	client.setUsername(split_msg[1]);
	client.setRealname(&split_msg[4][1]);
	for (size_t i = 5; i < split_msg.size(); i++)
	{
		client.setRealname(client.getRealname() + " " + split_msg[i]);
		if (i != split_msg.size() -1)
			client.setRealname(client.getRealname() + " ");
	}

	if (this->_clients.find(client.getRealname()) != this->_clients.end())
	{
		std::cout << PURPLE << client.getNickname() << R " has changed its user data to:\n"
		<< GREY " > username: " R << client.getUsername() << GREY "	-	realname: " R << client.getRealname() << std::endl;

		return (PURPLE "Your user data has been correctly updated" R);
	}

	client.setState(ACTIVE);
	std::cout << PURPLE << client.getNickname() << R " has completed registration\n"
	<< GREY " > username: " R << client.getUsername() << GREY " - realname: " R << client.getRealname() << std::endl;

	return (PURPLE "Welcome, " + client.getNickname() + R);
}

void	Server::broadcast(Client &client, Channel *channel, std::string const &msg)
{
	std::string output = PURPLE + client.getNickname() + ": " R + msg;
	std::cout << output << std::endl;

	if (!channel)
	{
		if (send(client.getClientFd(), output.c_str(), output.length(), 0) < 0)
			throw (std::runtime_error("Failed to send to socket " + client.getClientFd()));
		return;
	}

	std::vector<Client *>::iterator it = channel->members.begin();
	for (; it != channel->members.end(); ++it)
		if (&client != *it && (*it)->getState() == ACTIVE)
			if (send((*it)->getClientFd(), output.c_str(), output.length(), 0) < 0)
				throw (std::runtime_error("Failed to send to " + (*it)->getNickname()));
}

std::string Server::capCmd(Client &client, std::vector<std::string> &split_msg)
{
	if(split_msg.size() != 2 || split_msg[1] != "LS")
		return (RED "Invalid - expected: CAP LS" R);
	if (client.getState() == ACTIVE)
		return (PURPLE "You are already logged in" R);

	client.setState(ACTIVE);
	return (GREY "Capabilities set - you are now logged in!" R);
}
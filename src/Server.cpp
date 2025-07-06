/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 21:29:56 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/06 12:25:13 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

//----------------------Constructors/Destructors-------------------------------

// Constructs + sets up all server attributes
Server::Server(int port, std::string password): _port(port), _name("ircserv"), _password(password)
{
	this->setCmdMaps();
	this->_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (this->_fd < 0)
		throw (std::runtime_error("Server socket creation failed"));

	this->setSocket(htons(port), INADDR_ANY);

	this->printServer(NULL, GREY "Server has been properly set up");
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

	for (pollfd_iter it = this->_fds.begin(); it != this->_fds.end(); ++it)
		if (it->fd > 0)
			close(it->fd);
	this->_fds.clear();

	for (clients_iter it = this->_clients.begin(); it != this->_clients.end(); ++it)
		if (it->second)
			delete it->second;
	this->_clients.clear();

	for (channels_iter it = this->_channels.begin(); it != this->_channels.end(); ++it)
		if (it->second)
			delete it->second;
	this->_channels.clear();
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
		if (it->second != NULL && it->second->getFd() == fd)
			return (it->second);

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
		newclient->setState(AT_DOOR);
		this->_clients[newclient->getNickname()]= newclient;
	}

	this->_fds.push_back(newpoll);
}


/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 21:29:56 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/04 16:38:01 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, std::string ip): _port(port), _ip(ip)
{
	//Create + open socket fd
	this->_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (this->_fd < 1)
		throw (std::runtime_error("Server socket creation failed"));

	//Set socket options
	int opt = 1;
	setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	//Define socket address
	this->_addr.sin_family = AF_INET;
	this->_addr.sin_addr.s_addr = inet_addr(ip.c_str());
	this->_addr.sin_port = htons(port);

	this->_gen_addr = (struct sockaddr *)&this->_addr;
	this->_addrlen = sizeof(this->_addr);

	std::cout << BLUE "Server has been properly set up" << std::endl;
}

Server::~Server()
{
	//Close fds
	if (this->_fd > 0)
		close(this->_fd);

	std::map<int, Client>::iterator it = this->_clients.begin();
	for (; it != this->_clients.end(); ++it)
		if (it->first > 0)
			close(it->first);

	//clear containers
	this->_clients.clear();
	this->_fds.clear();
}

int Server::getFd()const
{
	return (this->_fd);
}

int	Server::getPort()const
{
	return (this->_port);
}

std::string	Server::getIp()const
{
	return (this->_ip);
}

Client &Server::getClient(int fd)
{
	if (this->_clients.find(fd) == this->_clients.end())
		throw (std::runtime_error("Client with corresponding fd not found"));

	return (this->_clients[fd]);
}

void	Server::initServer(int max_fds)
{
	if (bind(this->_fd, this->_gen_addr, this->_addrlen) < 0)
		throw (std::runtime_error("Server binding failed"));

	if (listen(this->_fd, max_fds) < 0)
		throw(std::runtime_error("Server fails to listen"));

	this->_fds.resize(max_fds);
	this->_fds[0].fd = this->_fd;
	this->_fds[0].events = POLLIN;
}

void	Server::readClient(int max_fds, int timeout)
{
	if (poll(&this->_fds[0], max_fds, timeout) < 0)
		throw (std::runtime_error("Poll failed"));

	static int openfds = 1;
	if (this->_fds[0].revents & POLLIN)
	{
		if (openfds == max_fds -1)
			throw (std::runtime_error("All client slots are taken!"));

		int newfd = accept(this->_fd, this->_gen_addr, &this->_addrlen);
		if (newfd < 0)
			throw (std::runtime_error("Server failed to accept client connection"));

		this->_clients[newfd] = Client(newfd);
		this->_fds[++openfds].fd = newfd;
		this->_fds[openfds].events = POLLIN;

		std::cout << GREY "New client connected on socket " << newfd << R << std::endl;
	}
}

void	Server::treatMsg(int buffsize)
{
	char buff[buffsize];
	pollfd_iter it = this->_fds.begin();

	for (it; it != this->_fds.end(); ++it)
	{
		if (it->fd <= 0 && it->revents & POLLIN)
		{
			memset(buff, 0, buffsize);
			std::cout << PURPLE "> Client " << it->fd << ": " R << buff << std::endl;

			int bytes_read = recv(it->fd, buff, buffsize, 0);
			if (bytes_read <= 0)
				this->removeClient(it);
			else
				send(it->fd, buff, bytes_read, 0);
		}
	}
}

void	Server::removeClient(pollfd_iter it)
{
	this->_clients.erase(it->fd);

	it->revents = NULL;
	it->events = NULL;
	close(it->fd);

	std::cout << PURPLE " has left" R << std::endl;
}

bool	Server::isActive()
{}

bool	Server::hasClient()
{}

Server::Server(Server const &src)
{
	throw (std::runtime_error("Copy constructor not allowed!"));
}

Server	&Server::operator=(Server const &src)
{
	throw (std::runtime_error("Assignment operator not allowed!"));
}

void	Server::setSocket()
{}

void	Server::pollErr()
{} //unsure yet

void	Server::pollUp()
{} //unsure yet

void	Server::sendMsg(std::string &msg)
{} //unsure

void	Server::acceptClient(int fd)
{} //unsure

void	Server::removeClient(int fd)
{
} //unsure

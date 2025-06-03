/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 21:29:56 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/03 22:12:48 by cbouvet          ###   ########.fr       */
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
	socklen_t addrlen = sizeof(this->_addr);
	struct sockaddr *gen_addr = (struct sockaddr *)&this->_addr;

	if (bind(this->_fd, gen_addr, addrlen) < 0)
		throw (std::runtime_error("Server binding failed"));

	if (listen(this->_fd, max_fds) < 0)
		throw(std::runtime_error("Server fails to listen"));

	this->_fds[0].fd = this->_fd;
	this->_fds[0].events = POLLIN;
}

void	Server::readClient(int max_fds, int timeout)
{
	if (poll(&this->_fds[0], max_fds, timeout) < 0)
		throw (std::runtime_error("Poll failed"));

	static int openfds = 0;
	if (this->_fds[0].revents & POLLIN)
	{
	}

}

void	Server::treatMsg(int buffsize)
{}

void	Server::removeClient(int fd)
{}

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

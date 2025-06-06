/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 21:29:56 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/06 19:47:54 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, std::string ip): _port(port), _ip(ip)
{
	//Create + open socket fd
	this->_active = false;
	this->_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (this->_fd < 0)
		throw (std::runtime_error("Server socket creation failed"));

	this->setSocket(htons(port), inet_addr(ip.c_str()));

	std::cout << GREY "Server has been properly set up" R << std::endl;
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

	this->addPoll(false);
	this->_active = true;
}

void	Server::handleClient(size_t max_fds, int buffsize, int timeout)
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

			this->addPoll(true);
		}

		this->treatMsg(buffsize);
	}
}

void	Server::treatMsg(int buffsize)
{
	if (this->_fds.size() <= 1)
		return ;

	char buff[buffsize];
	pollfd_iter it = this->_fds.begin() +1;

	for (; it != this->_fds.end(); ++it)
	{
		if (it->revents & POLLIN)
		{
			memset(buff, 0, buffsize);
			std::cout << PURPLE "> Client " << it->fd << ": " R;

			int bytes_read = recv(it->fd, buff, buffsize, 0);
			if (bytes_read <= 0)
				 it = this->removeClient(it);
			else
			{
				std::cout << buff << std::endl;
				send(it->fd, buff, bytes_read, 0);
			}
		}
		else if (it->revents & POLLERR)
		{
			perror("POLLERR: ");
		}
	}
}

typename Server::pollfd_iter	&Server::removeClient(pollfd_iter &it)
{
	pollfd_iter tmp = it;
	it--;
	this->_clients.erase(tmp->fd);
	close(tmp->fd);
	this->_fds.erase(tmp);

	std::cout << "has left" << std::endl;

	return (it);
}

bool	Server::isActive()
{
	return (this->_active);
}

bool	Server::hasClient()
{
	return (!this->_clients.empty());
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

void	Server::addPoll(bool isclient)
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

		this->_clients[newpoll.fd] = Client(newpoll.fd);
		std::cout << GREY "New client connected on socket " << newpoll.fd << R << std::endl;
	}

	this->_fds.push_back(newpoll);
}

void	Server::pollErr()
{
	//Handle POLLERR
	//unsure yet how to handle it
}

void	Server::pollUp()
{
	//Handle POLLUP
	//unsure yet how to handle it
}

void	Server::broadcast(std::string &msg)
{
	(void)msg;
	//Use to broadcast to all clients
} //unsure

void	Server::acceptClient(int fd)
{
	(void)fd;
	//This was recommended but since it's only 1 function for send, I'm unsure yet how useful this is
}

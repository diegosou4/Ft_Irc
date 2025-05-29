/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   classes.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 10:44:44 by cbouvet           #+#    #+#             */
/*   Updated: 2025/05/29 22:54:56 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "classes.hpp"

Server::Server(Client &client, int timeout): client(client), fd(client.fd)
{
	if (timeout < 0)
	{
		int opt = 1;
		setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	}

	std::cout << GREY "Using client fd " << this->fd << " as a server" R << std::endl;
}

Server::~Server()
{
	std::cout << "Shutting server down" << std::endl;
	if (this->fd >= 0)
		close(this->fd);
	if (this->fds)
		delete [] this->fds;
}

void	Server::setServer(int clients_nb)
{
	if (bind(this->fd, this->client.gen_addr, this->client.addrlen) < 0)
		throw (std::runtime_error("Server binding failed"));

	if (listen(this->fd, clients_nb) < 0)
		throw (std::runtime_error("Server fails to listen"));

	this->fds = new struct pollfd[clients_nb];
	this->fds[0].fd = this->fd;
	this->fds[0].events = POLLIN;

	this->openfds = 2;
}

void	Server::getInput(int clients_nb, int timeout)
{
	if (poll(this->fds, clients_nb, timeout) < 0)
		throw (std::runtime_error("Poll failed"));

	if (this->fds[0].revents & POLLIN)
	{
		this->fds[this->openfds].fd = accept(this->fd, this->client.gen_addr, &this->client.addrlen);

		if (this->fds[this->openfds].fd < 0)
			throw (std::runtime_error("Server failed to accept connection"));
		if (this->openfds >= clients_nb -1)
		{
			close(this->fds[this->openfds].fd);
			std::cout << RED "Max clients reached" R << std::endl;
		}
		else
		{
			this->fds[this->openfds].events = POLLIN;
			std::cout << GREY "New client connected" R << std::endl;
		}

		this->openfds++;
	}
}



Client::Client(int fd): fd(fd)
{
	if (this->fd < 0)
		throw (std::runtime_error("Client socket creation failed"));

	std::cout << GREY "Created client with open fd " << this->fd << R << std::endl;
}

Client::~Client()
{
	std::cout << "Disconnecting" << std::endl;
	if (this->fd >= 0)
		close(this->fd);
}

void	Client::setConnect(sa_family_t family, std::string ip, int port)
{
	this->addr.sin_family = family;
	this->addr.sin_addr.s_addr = inet_addr(ip.c_str());
	this->addr.sin_port = htons(port);

	this->gen_addr = (struct sockaddr *)&this->addr;
	this->addrlen = sizeof(this->addr);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TestClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 13:15:48 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/16 22:29:07 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TestClient.hpp"

TestClient::TestClient(int port, std::string ip): _port(port), _ip(ip)
{
	this->_connected = false;
	this->_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (this->_fd < 0)
		throw (std::runtime_error("Server socket creation failed"));

	this->setSocket(htons(port), inet_addr(ip.c_str()));

	std::cout << GREY "Client has been properly set up" R << std::endl;
}

TestClient::~TestClient()
{
	std::cout << PURPLE "Disconnecting - bye!" << std::endl;
	if (this->_fd > 0)
		close(this->_fd);
}

int TestClient::getFd()const
{
	return (this->_fd);
}

int TestClient::getPort()const
{
	return (this->_port);
}

std::string TestClient::getIp()const
{
	return (this->_ip);
}

void TestClient::connectClient()
{
	for (int attempt = 1; attempt <= 5; attempt++)
	{
		std::cout << "Connection attempt " << attempt << "/5" << std::endl;

		if (connect(this->_fd, this->_gen_addr, this->_addrlen) >= 0)
			break;

		perror("Connect error");
		sleep(1);
		if (attempt == 5)
			throw (std::runtime_error("Client failed to connect to server"));
	}

	this->_connected = true;
	std::cout << GREY "Successfully connected to server" R << std::endl;
}

void TestClient::getOnline()
{
	struct pollfd fds[2] =
	{
		this->setPollfd(this->_fd),
		this->setPollfd(STDIN_FILENO)
	};

	fcntl(this->_fd, F_SETFL, O_NONBLOCK);
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

	while (true)
	{
		if (poll(fds, 2, -1) < 0)
			throw (std::runtime_error("Poll failed"));

		if (fds[0].revents & POLLIN)
			this->receiveMsg();

		if (fds[1].revents & POLLIN)
			this->sendMsg();
	}
}

TestClient::TestClient(TestClient const &src)
{
	throw (std::runtime_error("Copy constructor not allowed!"));
	(void)src;
}

TestClient	&TestClient::operator=(TestClient const &src)
{
	throw (std::runtime_error("Assignment operator not allowed!"));

	(void)src;
	return (*this);
}

void TestClient::setSocket(in_port_t port, in_addr_t ip)
{
	this->_addr.sin_family = AF_INET;
	this->_addr.sin_addr.s_addr = ip;
	this->_addr.sin_port = port;

	this->_gen_addr = (struct sockaddr *)&this->_addr;
	this->_addrlen = sizeof(this->_addr);
}

struct pollfd TestClient::setPollfd(int fd)
{
	struct pollfd newpollfd;

	newpollfd.events = POLLIN;
	newpollfd.revents = 0;
	newpollfd.fd = fd;

	if (newpollfd.fd < 0)
		throw (std::runtime_error("Error on socket " + fd));

	return (newpollfd);
}

void	TestClient::receiveMsg()
{
	char buff[BUFFSIZE];
	memset(buff, 0, BUFFSIZE);

	int bytes = recv(this->_fd, buff, BUFFSIZE, 0);
	if (!bytes)
		throw (std::runtime_error("Server disconnected"));
	if (bytes <= 0)
		throw (std::runtime_error(strerror(errno)));
	else
	{
		buff[bytes] = '\0';
		std::cout << "\r" << buff << "\n" << std::flush;
	}
}

void	TestClient::sendMsg()
{
	std::string input;
	getline(std::cin, input);

	if (!input.empty())
		if (send(this->_fd, input.c_str(), input.size(), 0) <= 0)
			throw (std::runtime_error("Failed to send message"));
}

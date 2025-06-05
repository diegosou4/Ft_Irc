/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TestClient.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 13:15:48 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/05 13:50:09 by cbouvet          ###   ########.fr       */
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
	std::cout << PURPLE "Successfully connected to server" R << std::endl;
}

uint32_t TestClient::sendMsg(int buffsize)
{
	uint32_t totalbytes = 0;
	char buff[buffsize];

	while (this->_connected)
	{
		memset(buff, 0, buffsize);
		std::string input;

		std::cout << "> ";
		getline(std::cin, input);

		if (input.empty())
			break;
		else
		{
			send(this->_fd, input.c_str(), input.size(), 0);

			int bytes = recv(this->_fd, buff, buffsize, 0);
			totalbytes += bytes;
			if (bytes)
				std::cout << GREY "sent" R << std::endl;
		}
	}

	return (totalbytes);
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


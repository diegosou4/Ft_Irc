/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 21:29:56 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/12 12:28:50 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, std::string ip): _port(port), _ip(ip)
{
	this->_active = false;
	this->_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (this->_fd < 0)
		throw (std::runtime_error("Server socket creation failed"));

	this->setSocket(htons(port), inet_addr(ip.c_str()));

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

	this->clientDataConfig();
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

std::string	Server::getIp()const
{
	return (this->_ip);
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
	this->_active = true;
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

		this->treatMsg();
	}
}

void	Server::treatMsg()
{
	if (this->_fds.size() <= 1)
		return ;

	pollfd_iter it = this->_fds.begin() +1;

	for (; it != this->_fds.end(); ++it)
	{
		switch (it->revents)
		{
			case POLLHUP:
				this->pollHup(it); break;
			case POLLIN:
				this->pollIn(it); break;
			case POLLERR:
				this->pollErr(it); break;
			case POLLNVAL:
				this->pollNVal(); break;
			default:
				break;
		}
	}
}

typename Server::pollfd_iter	&Server::removeClient(pollfd_iter &it)
{
	this->broadcast(this->_online[it->fd]->username, "has left");

	pollfd_iter tmp = it;
	it--;

	delete this->_online[tmp->fd];
	this->_online.erase(tmp->fd);

	close(tmp->fd);
	this->_fds.erase(tmp);

	return (it);
}

bool	Server::isActive()
{
	return (this->_active);
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

		Client *newclient = new Client(newpoll.fd);
		this->_online[newpoll.fd] = newclient;

		this->welcomeScreen(*newclient, newpoll);

		this->broadcast(newclient->username, "has just connected");
	}

	this->_fds.push_back(newpoll);
}

void	Server::welcomeScreen(Client const &client, struct pollfd &newpoll)
{
	std::stringstream ss;

	ss << PURPLE WELCOME R << std::endl;
	ss << GREY << "1 - Log in	|	2 - Register\n" R << std::endl;

	std::string msg;
	while (getline(ss, msg))
	{
		if (send(client.fd, msg.c_str(), msg.length(), 0) < 0)
			throw std::runtime_error("Failed to send welcome message");
	}

	if (newpoll.revents & POLLIN)
	{
		recv()
	}
}

bool	Server::clientRegister()
{
	return (true);
}

bool	Server::clientLogIn()
{
	return (true);
}

void	Server::homeScreen()
{}

void	Server::pollIn(pollfd_iter &it)
{
	char buff[BUFFSIZE];
	memset(buff, 0, BUFFSIZE);

	int bytes_read = recv(it->fd, buff, BUFFSIZE, 0);

	if (bytes_read < 0)
		broadcast(this->_online[it->fd]->username, strerror(errno));
	if (bytes_read <= 0)
		it = this->removeClient(it);
	else
		this->broadcast(this->_online[it->fd]->username, buff);
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
	this->removeClient(it);
	/* to add:
		-> remove client form active conversations*/
}

void	Server::pollNVal()
{
	std::cout << "POLLNVAL triggered" << std::endl;
	throw (std::runtime_error("Invalid socket descriptor"));
}

void	Server::broadcast(std::string const &user, std::string const &msg)
{
	std::string output = PURPLE + user + R + ": " + msg;
	std::cout << output << std::endl;

	online_iter it = this->_online.begin();
	for (; it != this->_online.end(); ++it)
		if (user != it->second->username)
			if (send(it->first, output.c_str(), output.length(), 0) < 0)
				throw (std::runtime_error("Failed to send to " + it->second->username));
}

void	Server::clientDataConfig()
{
	if (this->_clients.empty())
		return ;

	std::string config_dir = std::string(getenv("HOME")) + "/.config/my_irc/";
	system(("mkdir -p " + config_dir).c_str());

	std::string config_file = config_dir + "/clients.csv";
	std::ofstream file(config_file.c_str());

	if (!file.is_open())
		throw (std::runtime_error("Failed to open config file " + config_file));

	file << "username,password\n"; // can add nickname, ip, channels, etc.

	for (clients_iter it = this->_clients.begin(); it != this->_clients.end(); ++it)
		if (it->second)
			file	<< it->second->username << ","
					<< it->second->password << "\n";

	file.close();
}

void	Server::clientDataRetrieve()
{
	std::string config_file = std::string(getenv("HOME")) + "/.config/my_irc/clients.csv";
	std::ifstream file(config_file.c_str());

	if (!file.is_open())
		return ;

	std::string uname;
	std::string pass;

	getline(file, uname);
	while (!uname.empty())
	{
		getline(file, uname, ',');
		getline(file, pass, '\n');

		Client *client  = new Client();
		client->username = uname;
		client->password = pass;

		this->_clients[uname] = client;
	}

	file.close();
}

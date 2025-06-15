/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 21:29:56 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/15 17:40:18 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(int port, std::string ip): _port(port), _ip(ip)
{
	this->clientDataRetrieve();

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
		if (it->fd == REMOVAL)
			this->_fds.erase(it--);
	}
}

void	Server::removeClient(Client *client)
{
	if (!client)
		return;

	if (this->_clients.find(client->username) != this->_clients.end())
		this->_clients[client->username]->state = OFFLINE;

	for (pollfd_iter it = this->_fds.begin(); it != this->_fds.end(); ++it)
		if (client->fd == it->fd)
			it->fd = REMOVAL;

	online_iter online_client = this->_online.find(client->fd);
	close(online_client->first);
	this->_online.erase(online_client);

	this->broadcast(client->username, "has left");
	delete client;
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
		newclient->state = ONLINE;
		this->_online[newpoll.fd] = newclient;

		this->welcomeScreen(newpoll);

		this->broadcast(newclient->username, "has just connected");
	}

	this->_fds.push_back(newpoll);
}

void	Server::welcomeScreen(struct pollfd &newpoll)
{
	std::string msg = 	PURPLE WELCOME R "\n"
						GREY "1 - Log in    |    2 - Register" R;

	if (send(newpoll.fd, msg.c_str(), msg.length(), 0) < 0)
		throw std::runtime_error("Failed to send welcome message");
}

void	Server::pollIn(pollfd_iter &it)
{
	Client *client = this->_online[it->fd];

	switch (client->state)
	{
		case ONLINE:
			this->chooseAuth(*client); break;
		case REG_USERNAME:
			this->regUsername(*client); break;
		case REG_PASS:
			this->regPass(*client); break;
		case REG_PASSCONFIRM:
			this->regPassConfirm(*client); break;
		case LOG_USERNAME:
			this->logUsername(*client); break;
		case LOG_PASS:
			this->logPass(*client); break;
		case ACTIVE:
			this->chatMsg(it);
		default:
			break;
	}
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

void	Server::chooseAuth(Client &client)
{
	std::string msg = getMsg(client);
	if (msg.empty())
		return ;

	std::string output;
	if (msg == "1")
	{
		client.state = LOG_USERNAME;
		output = PURPLE + std::string("\nLOG IN\n") + GREY "Username: " R;
	}
	else if (msg == "2")
	{
		client.state = REG_USERNAME;
		output = PURPLE + std::string("\nREGISTER\n") + GREY "Username: " R;
	}

	if (send(client.fd, output.c_str(), output.length(), 0) < 0)
		throw (std::runtime_error("Failed to send to socket " + client.fd));
}

void	Server::regUsername(Client &client)
{
	std::string msg = getMsg(client);
	if (msg.empty())
		return ;

	std::string output;

	if (!this->_clients.empty() && this->_clients.find(msg) != this->_clients.end())
		output = RED + std::string("Username already taken - Please choose a different username")
		+ GREY + "\nUsername: " + R;
	else
	{
		client.username = msg;
		client.state = REG_PASS;
		output = GREY + std::string("Password: ") + R;
	}

	if (send(client.fd, output.c_str(), output.length(), 0) < 0)
		throw (std::runtime_error("Failed to send to soscket " + client.fd));
}

void	Server::regPass(Client &client)
{
	std::string msg = getMsg(client);
	if (msg.empty())
		return ;

	std::string output = RED + std::string("Password must contain at least ");

	if (msg.size() < 4)
		output = output + "4 characters" + GREY + "\nPassword: " + R;
	else if (msg.find_first_of(DIGIT_CHAR) == msg.npos)
		output = output + "one digit" + GREY + "\nPassword: " + R;
	else if (msg.find_first_of(ALPHA_CHAR) == msg.npos)
		output = output + "one uppercase or lowercase letter" + GREY + "\nPassword: " + R;
	else
	{
		client.password = msg;
		client.state = REG_PASSCONFIRM;
		output = GREY + std::string("Confirm passowrd: ") + R;
	}

	if (send(client.fd, output.c_str(), output.length(), 0) < 0)
		throw (std::runtime_error("Failed to send to soscket " + client.fd));
}

void	Server::regPassConfirm(Client &client)
{
	std::string msg = getMsg(client);
	if (msg.empty())
		return ;

	std::string output;

	if (msg != client.password)
	{
		output = RED + std::string("Password don't match, please try again.") + GREY "\nPassword:" R;
		client.state = REG_PASS;
	}
	else
	{
		output = PURPLE "\n * Account registered correctly * \n" R;
		client.state = ACTIVE;
		this->_clients.insert(std::make_pair(client.username, new Client(client)));
	}

	if (send(client.fd, output.c_str(), output.length(), 0) < 0)
		throw (std::runtime_error("Failed to send to soscket " + client.fd));

	if (client.state == ACTIVE)
		this->homeScreen(client);
}

void	Server::logUsername(Client &client)
{
	std::string msg = getMsg(client);
	if (msg.empty())
		return ;

	std::string output;
	if (this->_clients.empty() || this->_clients.find(msg) == this->_clients.end())
	{
		client.state = ONLINE;
		output = RED + std::string("Account doesn't exist")
		+ GREY "\n\n1 - Log in    |    2 - Register" R;
	}
	else
	{
		client.state = LOG_PASS;
		output = GREY + std::string("Password:") + R;
	}

	if (send(client.fd, output.c_str(), output.length(), 0) < 0)
		throw (std::runtime_error("Failed to send to soscket " + client.fd));
}

void	Server::logPass(Client &client)
{
	std::string msg = getMsg(client);
	if (msg.empty())
		return ;

	std::string output;
	if (msg != this->_clients[client.username]->password)
	{
		client.state = ONLINE;
		client.username = "";
		output = RED + std::string("Incorrect password!")
		+ GREY "\n\n1 - Log in    |    2 - Register" R;
	}
	else
	{
		client.state = ACTIVE;
		this->_clients[client.username]->state = ACTIVE;
		output = PURPLE + std::string("\n * Successfully logged in* \n") + R;
	}

	if (send(client.fd, output.c_str(), output.length(), 0) < 0)
		throw (std::runtime_error("Failed to send to soscket " + client.fd));
}

void	Server::homeScreen(Client &client)
{
	(void)client;
}

void	Server::chatMsg(pollfd_iter &it)
{
	std::string msg = this->getMsg(*this->_online[it->fd]);

	if (msg.empty())
		std::cout << "detected msg is empty";

	this->broadcast(this->_online[it->fd]->username, msg);
}

std::string Server::getMsg(Client &client)
{
	char buff[BUFFSIZE];
	memset(buff, 0, BUFFSIZE);

	int bytes_read = recv(client.fd, buff, BUFFSIZE, 0);

	if (bytes_read <= 0)
	{
		if (bytes_read < 0)
			broadcast(client.username, strerror(errno));
		this->removeClient(&client);
		buff[0] = 0;
	}

	return (std::string(buff));
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

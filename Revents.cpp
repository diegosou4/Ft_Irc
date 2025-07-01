/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Revents.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 12:41:33 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/29 23:37:37 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//----------------------------Poll/revents-------------------------------------
#include "Server.hpp"

// Loops around all pollfds for revent activity - if found, sends to relevant event-managing method
void	Server::treatRevent()
{
	if (this->_fds.size() <= 1)
		return ;

	for (pollfd_iter it = this->_fds.begin() +1; it != this->_fds.end(); ++it)
	{
		Client *client = this->getClient(it->fd);
		if (!client)
			continue ;
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
			this->_fds.erase(it--);
	}
}

// POLLUP = client left
void	Server::pollHup(Client &client)
{
	this->cmdQuit(&client, NULL, newVector("QUIT", ":Dropped unexpectedly", 0));
}

// POLLIN = message sent by client
// Retrieves message, splits it into vector, sends it to command managers
void Server::pollIn(Client &client)
{
	// Need to modify function to handle newlines DIEGO
	std::string msg = getMsg(client);

	if (msg.empty())
		return;
	// Split msg by "\r\n" manually
	std::vector<std::string> split_enter;
	size_t start = 0;
	size_t end = 0;
	
	while ((end = msg.find("\r\n", start)) != std::string::npos) { // Camille Check This
		std::string line = msg.substr(start, end - start);
		if (!line.empty())
			split_enter.push_back(line);
		start = end + 2;
	}
	if (start < msg.size()) {
		std::string line = msg.substr(start);
		if (!line.empty())
			split_enter.push_back(line);
	}

	for (size_t i = 0; i < split_enter.size(); ++i)
	{
		std::string &line = split_enter[i];
		std::cout << "Processing line: " << line << std::endl;
		std::vector<std::string> cmdArgs = this->splitMsg(line);
		
		if (cmdArgs.empty())
			continue;

		Channel *channel = findChannel(cmdArgs); 
		std::string cmdName = cmdArgs[0];
		if (this->_authcmds.find(cmdName) != this->_authcmds.end())
			(this->*_authcmds[cmdName])(&client, channel, cmdArgs);
		else 
			this->sendNumeric(client, ERR_UNKNOWNCOMMAND);
	}
	
}

// POLLERR = error occurred with fd
// Depending on error code: ignores, reopen socket, or removes client
void	Server::pollErr(Client &client)
{
	this->printServer(&client, RED  "Error occurred: ");

	if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
	{
		this->printServer(NULL, RED "Retrying ...");
		return ;
	}
	else if (errno == ETIMEDOUT)
	{
		this->printServer(NULL, RED "Reopening attempt ...");
		close(client.getFd());

		client.setFd(socket(AF_INET, SOCK_STREAM, 0));
		if (client.getFd() >= 0)
		{
			fcntl(client.getFd(),  F_SETFL, O_NONBLOCK);
			this->printServer(NULL, PURPLE "Reconnection successful");
			return ;
		}
	}

	this->printServer(NULL, RED "Unrecoverable - closing socket");
	this->cmdQuit(&client, NULL, newVector("QUIT", ":Dropped unexpectedly", 0));
}

// POLLNVAL = file descriptor issue
// Shows error message + sends to client removal
void	Server::pollNVal(Client &client)
{
	this->printServer(&client, RED "Invalid file descriptor\nUnrecoverable - closing socket");
	this->cmdQuit(&client, NULL, newVector("QUIT", ":Dropped unexpectedly", 0));
}

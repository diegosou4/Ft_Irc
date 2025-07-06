/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Revents.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 12:41:33 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/06 10:15:26 by feden-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//----------------------------Poll/revents-------------------------------------
#include "../includes/Server.hpp"

// Loops around all pollfds for revent activity - if found, sends to relevant event-managing method

void Server::treatRevent()
{
	if (this->_fds.size() <= 1)
		return;

	for (size_t i = this->_fds.size() - 1; i > 0; --i)
	{
		if (this->_clients.empty())
			return;

		Client *client = this->getClient(this->_fds[i].fd);
		if (!client)
			continue;

		switch (this->_fds[i].revents)
		{
			case POLLHUP: this->pollHup(*client); break;
			case POLLIN: this->pollIn(*client); break;
			case POLLERR: this->pollErr(*client); break;
			case POLLNVAL: this->pollNVal(*client); break;
			default: break;
		}

		if (this->_fds[i].fd == REMOVAL)
		this->_fds.erase(this->_fds.begin() + i);
	}
}

// POLLUP = client left
void	Server::pollHup(Client &client)
{
	this->cmdQuit(&client, NULL, newVector("QUIT", ":Dropped unexpectedly", 0));
}

// POLLIN = message sent by client
// Retrieves message, treats each line, splits it into vector, sends it to command managers
void Server::pollIn(Client &client)
{
	std::string msg = getMsg(client);
	if (msg.empty())
		return;

	std::string line;
	std::stringstream ss(msg);

	while (getline(ss, line, '\n'))
	{
		if (!line.empty() && line[line.length() - 1] == '\r')
			line = line.substr(0, line.length() -1);

		str_vector split_msg = this->splitMsg(line);
		if (line.empty() || split_msg.empty())
			continue;

		for (size_t i = 0; split_msg[0][i]; i++)
			split_msg[0][i] = toupper(split_msg[0][i]);

		Channel *channel = findChannel(split_msg);

		if (this->_authcmds.find(split_msg[0]) != this->_authcmds.end())
			(this->*_authcmds[split_msg[0]])(&client, channel, split_msg);
		else if (split_msg[0] == "CAP" || split_msg[0] == "WHO")
			continue;
		else
			this->sendNumeric(client, 0, ERR_UNKNOWNCOMMAND);
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

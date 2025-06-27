/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Revents.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 12:41:33 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/27 20:33:58 by cbouvet          ###   ########.fr       */
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
// Sends to client removal function
void	Server::pollHup(Client &client)
{
	this->removeClient(client);
}

// POLLIN = message sent by client
// Retrieves message, splits it into vector, sends it to command managers
void Server::pollIn(Client &client)
{
	std::string msg = getMsg(client);
	if (msg.empty())
		return;

	int code = 0;
	std::vector<std::string> split_msg = this->splitMsg(msg);
	Channel *channel = findChannel(split_msg);

	if (this->_authcmds.find(split_msg[0]) != this->_authcmds.end())
		code = (this->*_authcmds[split_msg[0]])(&client, channel, split_msg);
	else if (!channel)
			code = ERR_NOSUCHCHAN;
	else if (this->_chancmds.find(split_msg[0]) != this->_chancmds.end())
		code = (channel->*_chancmds[split_msg[0]])(client, split_msg);
	else
		code = ERR_UNKNOWNCOMMAND;

	if (!code)
		this->broadcast(client, *channel, split_msg[0], msg);
	else
		this->sendNumeric(client, code);
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
	this->removeClient(client);
}

// POLLNVAL = file descriptor issue
// Shows error message + sends to client removal
void	Server::pollNVal(Client &client)
{
	this->printServer(&client, RED "Invalid file descriptor\nUnrecoverable - closing socket");

	this->removeClient(client);
}

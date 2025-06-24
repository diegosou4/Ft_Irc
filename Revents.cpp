/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Revents.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 12:41:33 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/20 12:47:36 by cbouvet          ###   ########.fr       */
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

	std::vector<std::string> split_msg = this->splitMsg(msg);
	Channel *channel = findChannel(split_msg);

	std::string output;

	if (this->_authcmds.find(split_msg[0]) != this->_authcmds.end())
		output = (this->*_authcmds[split_msg[0]])(&client, channel, split_msg);
	else if (channel && this->_chancmds.find(split_msg[0]) != this->_chancmds.end())
		output = (channel->*_chancmds[split_msg[0]])(client, split_msg);
	else
		output = RED "Invalid - command not recognised" R;

	this->broadcast(client, channel, output);
}

// POLLERR = error occurred with fd
// Depending on error code: ignores, reopen socket, or removes client
void	Server::pollErr(Client &client)
{
	std::cerr << RED "Error occurred with client " << client.nickname << ":" << strerror(errno) << R << std::endl;

	if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
	{
		std::cout << "Retrying ..." << std::endl;
		return ;
	}
	else if (errno == ETIMEDOUT)
	{
		std::cout << "Reopening attempt ..." << std::endl;
		close(client.fd);

		client.fd = socket(AF_INET, SOCK_STREAM, 0);
		if (client.fd >= 0)
		{
			fcntl(client.fd,  F_SETFL, O_NONBLOCK);
			std::cout << PURPLE "Reconnection successful" R << std::endl;
			return ;
		}
	}

	std::cerr << RED "Unrecoverable - closing socket" R << std::endl;
	this->removeClient(client);
}

// POLLNVAL = file descriptor issue
// Shows error message + sends to client removal
void	Server::pollNVal(Client &client)
{
	std::cerr << RED "File descriptor " << client.fd << " is invalid" R << std::endl;
	std::cerr << RED "Unrecoverable - closing socket" R << std::endl;

	this->removeClient(client);
}

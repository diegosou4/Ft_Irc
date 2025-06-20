/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 13:00:56 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/20 13:11:43 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//--------------------------------Utils----------------------------------------
#include "Server.hpp"


// Creates maps linking command to an function
void	Server::setCmdMaps()
{
	this->_authcmds["PASS"] = &Server::passCheck;
	this->_authcmds["NICK"] = &Server::nickCheck;
	this->_authcmds["USER"] = &Server::userCheck;
	this->_authcmds["JOIN"] = &Server::joinCheck;

	this->_chancmds["MODE"] = &Channel::modeCmd;
	this->_chancmds["TOPIC"] = &Channel::topicCmd;
	this->_chancmds["INVITE"] = &Channel::inviteCmd;
	this->_chancmds["PRIVMSG"] = &Channel::privmsgCmd;
	this->_chancmds["KICK"] = &Channel::kickCmd;
}

// Displays welcome screen to user + notifies server
void	Server::welcomeScreen(Client &client)
{
	std::cout << PURPLE << client.nickname << R " is at the door" << std::endl;

	this->broadcast(client, NULL, PURPLE WELCOME GREY INSTRUCTIONS R);
}

// Retrieves message from user + perform checks - removes client if errors
std::string Server::getMsg(Client &client)
{
	char buff[BUFFSIZE];
	memset(buff, 0, BUFFSIZE);

	int bytes_read = recv(client.fd, buff, BUFFSIZE, 0);

	if (bytes_read <= 0)
	{
		if (bytes_read < 0)
			broadcast(client, NULL, strerror(errno));
		this->removeClient(client);
		buff[0] = 0;
	}

	return (std::string(buff));
}

// Splits user command into vector string items
std::vector<std::string> Server::splitMsg(std::string &msg)
{
	std::vector<std::string> split_msg;
	size_t pos = 0;

	while (pos <= msg.size())
	{
		pos = msg.find_first_of(" \t\0");
		split_msg.push_back(msg.substr(0, pos));
		if (pos == msg.size())
			break;
		msg = msg.substr(pos +1, msg.size());
	}

	return (split_msg);
}

Channel	*Server::findChannel(std::vector<std::string> &split_msg)
{
	Channel *channel = NULL;
	std::string channel_name;

	if (split_msg.size() >= 2 && split_msg[1][0] == '#')
		channel_name = split_msg[1][0];
	else if (split_msg.size() >= 3 && split_msg[2][0] == '#' && split_msg[0] == "INVITE")
		channel_name = split_msg[2][0];

	if (this->_channels.find(channel_name) != this->_channels.end())
		channel = this->_channels[channel_name];

	return (channel);
}

// Broadcasts message to server, client, & channel if applicable
void	Server::broadcast(Client &client, Channel *channel, std::string const &msg)
{
	if (!channel)
	{
		if (send(client.fd, msg.c_str(), msg.length(), 0) < 0)
			throw (std::runtime_error("Failed to send to " + client.nickname));
		return;
	}

	std::string output = PURPLE + client.nickname + ": " R + msg;
	std::cout << output << std::endl;

	std::vector<Client *>::iterator it = channel->members.begin();
	for (; it != channel->members.end(); ++it)
		if (&client != *it && (*it)->state == ACTIVE)
			if (send((*it)->fd, output.c_str(), output.length(), 0) < 0)
				throw (std::runtime_error("Failed to send to " + (*it)->nickname));
}

// Removes client from pollfd, sets client as OFFLINE, closes fd, broadcast departure
void	Server::removeClient(Client &client)
{
	for (pollfd_iter it = this->_fds.begin(); it != this->_fds.end(); ++it)
		if (client.fd == it->fd)
			it->fd = REMOVAL;

	for (channels_iter it = this->_channels.begin(); it != this->_channels.end(); ++it)
		if (std::find(it->second->members.begin(), it->second->members.end(), &client) != it->second->members.end())
			this->broadcast(client, it->second, "has left");

	close(client.fd);
	client.state = OFFLINE;
}

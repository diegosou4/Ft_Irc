/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 13:00:56 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/27 13:53:57 by cbouvet          ###   ########.fr       */
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
	this->printServer(&client, "is at the door");
	this->broadcast(client, NULL, WELCOME INSTRUCTIONS);
}

// Retrieves message from user + perform checks - removes client if errors
std::string Server::getMsg(Client &client)
{
	char buff[BUFFSIZE];
	memset(buff, 0, BUFFSIZE);

	int bytes_read = recv(client.getFd(), buff, BUFFSIZE, 0);

	if (bytes_read <= 0)
	{
		if (bytes_read < 0)
			this->printServer(&client, RED + std::string(strerror(errno)));
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
		channel_name = split_msg[1];
	else if (split_msg.size() >= 3 && split_msg[2][0] == '#' && split_msg[0] == "INVITE")
		channel_name = split_msg[2];

	if (this->_channels.find(channel_name) != this->_channels.end())
		channel = this->_channels[channel_name];

	return (channel);
}

// Removes client from pollfd, sets client as OFFLINE, closes fd, broadcast departure
void	Server::removeClient(Client &client)
{
	for (pollfd_iter it = this->_fds.begin(); it != this->_fds.end(); ++it)
		if (client.getFd() == it->fd)
			it->fd = REMOVAL;

	for (channels_iter it = this->_channels.begin(); it != this->_channels.end(); ++it)
		if (std::find(it->second->getMembers().begin(), it->second->getMembers().end(), &client) != it->second->getMembers().end())
			this->broadcast(client, it->second, " has left");

	this->printServer(&client, "has left");

	close(client.getFd());
	client.setState(OFFLINE);
}

void	Server::printServer(Client *client, std::string const &msg)
{
	if (client)
		std::cout << PURPLE << client->getNickname()<< ": " R;

	std::cout << msg << R << std::endl;
}

void	Server::sendClient(Client &client, std::string const &msg)
{
	if (send(client.getFd(), msg.c_str(), msg.length(), 0) < 0)
		this->printServer(&client, RED "Failed to send message");
}

void	Server::sendNumeric(Client &client, int code, std::string &msg)
{
	std::stringstream ss;
	std::string msg;

	if (ErrMsg.find(code) != ErrMsg.end())
		msg = ErrMsg.find(code)->second;

	ss 	<< ":" << this->_name << " " \
		<< std::setw(3) << std::setfill('0') \
		<< code << " " << client.getNickname() \
		<< " " << msg << "\r\n" << std::endl;

	this->sendClient(client, ss.str());
}
void	Server::broadcast(Client &client, Channel &channel, std::string &cmd, std::string const &msg)
{
	std::string output = client.getPrefix() + " " + cmd + " " + msg + "\r\n";

	Channel::member_iter it = channel.getMembers().begin();
	for (; it != channel.getMembers().end(); ++it)
		if (*it != &client && (*it)->getState() == ACTIVE)
			this->sendClient(**it, msg);
}



/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 13:00:56 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/29 14:25:26 by cbouvet          ###   ########.fr       */
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
	this->_authcmds["PRIVMSG"] = &Server::privMsgCheck;

	this->_chancmds["MODE"] = &Channel::modeCmd;
	this->_chancmds["TOPIC"] = &Channel::topicCmd;
	this->_chancmds["INVITE"] = &Channel::inviteCmd;
	this->_chancmds["KICK"] = &Channel::kickCmd;
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
	//add looking for user if privmsg
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

void	Server::sendNumeric(Client &client, int code)
{
	std::stringstream ss;
	std::string msg;

	if (ErrMsg.find(code) == ErrMsg.end())
		throw (std::runtime_error("Invalid error code"));

	msg = ErrMsg.find(code)->second;

	ss 	<< ":" << this->_name << " " \
		<< std::setw(3) << std::setfill('0') \
		<< code << " " << client.getNickname() \
		<< " " << msg << "\r\n";

	this->sendClient(client, ss.str());
}

void	Server::sendNumeric(Client &client, int code, std::string const &msg)
{
	std::stringstream ss;

	ss 	<< ":" << this->_name << " " \
		<< std::setw(3) << std::setfill('0') \
		<< code << " " << client.getNickname() \
		<< " " << msg << "\r\n";

	this->sendClient(client, ss.str());
}

void	Server::broadcast(Client &client, Channel &channel, std::string &cmd, std::string const &msg)
{
	std::string output = client.getPrefix() + " " + cmd + " " + msg + "\r\n";

	Channel::member_iter it = channel.getMembers().begin();
	for (; it != channel.getMembers().end(); ++it)
		if (*it != &client && (*it)->getState() == ACTIVE)
			this->sendClient(**it, output);
}

void	Server::broadcast(Client &client, std::string &cmd, std::string const &msg)
{
	channels_iter it = this->_channels.begin();
	for (; it != this->_channels.end(); ++it)
		if (it->second->isMember(client))
			broadcast(client, *it->second, cmd, msg);
}

void	Server::broadcast(Client &client, Client &target, std::string &cmd, std::string const &msg)
{
	std::string output = client.getPrefix() + " " + cmd + " " + msg + "\r\n";
	this->sendClient(target, output);
}


bool	Server::authCheck(Client &client)
{
	if (client.getState() == ACTIVE)
		return (true);

	if (client.getState() != PASS_OK || !client.passedNick() || !client.passedUser())
		return (false);

	client.setState(ACTIVE);
	this->_clients[client.getNickname()] = &client;

	this->sendNumeric(client, RPL_WELCOME, WELCOME);
	this->printServer(&client, "has successfully logged in");

	return (true);
}

int		Server::cmdCheck(Client *client, Channel *channel, std::string target)
{
	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->getState() != ACTIVE)
		return (ERR_NOTAUTHED);
	else if (!target.empty() && this->_clients.find(target) == this->_clients.end())
		return (ERR_NOSUCHNICK);
	else if (!channel)
		return(ERR_NOSUCHCHAN);
	else if (!channel->isMember(*client))
		return (ERR_NOTINCHAN);

	return (SUCCESS);
}

int		Server::checkModeFormat(std::vector<std::string> &msg)
{
	std::string flags = "ilkot";
	std::string valid_signs = "+-";

	if (msg.size() > 2 && !strchr(valid_signs.c_str(), msg[2][0]))
		return (-1);

	for (int i = 2; i < msg.size(); i++)
	{
		if (i != 2 && !strchr(valid_signs.c_str(), msg[i][0]))
			return (i);
		if (msg[i].find_first_not_of(flags + valid_signs) != msg[i].npos)
			return (-1);
		for (int j = 0; msg[i][j]; j++)
			if (strchr(valid_signs.c_str(), msg[i][j]) && (!msg[i][j +1] || !strchr(flags.c_str(), msg[i][j +1])))
				return (-1);
	}

	return (msg.size());
}

void	Server::sendMode(Client &client, Channel &channel, int stop, std::vector<std::string> &msg)
{
	char sign = msg[2][0];
	std::string valid_signs = "+-";

	for (int i = 2; i < stop; i++)
	{
		for (size_t j = 0; msg[i][j]; j++)
		{
			if (strchr(valid_signs.c_str(), msg[i][j]))
				sign = msg[i][j]; continue;

			std::string arg;
			if (stop < msg.size() && channel.needsArg(sign, msg[i][j]))
				arg = " " + msg[stop++];
			int code = channel.modeFlags(client, sign, msg[i][j], &arg[1]);
			if (code)
				this->sendNumeric(client, code);
			else
				this->broadcast(client, channel, msg[0], std::string(1, sign) + msg[i][j] + arg);
		}
	}
}



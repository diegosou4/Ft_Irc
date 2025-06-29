/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChanCmds.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 21:35:58 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/29 21:44:02 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//------------------------------ChanCmds---------------------------------------
#include "Server.hpp"

// Performs checks, adds channel didnt exist, adds user to channel, sends relevant messages
void Server::cmdJoin(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	if (!client)
		throw (std::runtime_error("Fatal: client not found"));
	if (client->getState() != ACTIVE)
		code = ERR_NOTAUTHED;
	else if (msg.size() != 2)
		code = ERR_NEEDMOREPARAMS;
	else if (msg[1][0] != '#' || msg[1].length() < 2)
		code = ERR_UNKNOWNCOMMAND;

	if (!code)
	{
		if (!channel)
		{
			channel = new Channel(msg[1]);
			this->_channels[msg[1]] = channel;
		}
		code = channel->addMember(*client);
	}

	if (code)
		return (this->sendNumeric(*client, code));

	this->cmdTopic(client, channel, this->newVector("TOPIC", channel->getName(), 0));
	this->cmdNames(client, channel, this->newVector("NAMES", channel->getName(), 0));

	this->broadcast(*client, *channel, msg[0], channel->getName());
}

// Performs checks, sends to channel invite method, sends relevant messages
void Server::cmdInvite(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	if (msg.size() != 3)
		code = ERR_NEEDMOREPARAMS;
	else
		code = cmdCheck(client, channel, msg[1]);

	if (!code)
		code = channel->setInvited(msg[1]);

	if (code)
		return (this->sendNumeric(*client, code));

	Client target = *this->_clients[msg[1]];

	this->sendNumeric(*client, RPL_INVITING, target.getNickname() + " " + channel->getName());
	this->broadcast(*client, target, msg[0], target.getNickname() + " :" + channel->getName());
}

// Performs checks, sends to channel kick method, sends relevant message
void Server::cmdKick(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;
	std::string reason;

	if (msg.size() < 3)
		code = ERR_NEEDMOREPARAMS;
	else if (msg.size() > 3 && (msg[3][0] != ':' || msg[3].length() < 2))
		code = ERR_NEEDMOREPARAMS;
	else
		code = cmdCheck(client, channel, msg[2]);

	if (!code)
		code = channel->kickMember(*client, msg[2]);

	if (code)
		return (this->sendNumeric(*client, code));

	if (msg.size() > 3)
		reason = this->unSplit(msg, 3);
	this->broadcast(*client, *channel, msg[0], msg[1] + reason);
}

// Performs checks, sends to channel remove method, deletes channel if empty, sends relevant message
void Server::cmdPart(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;
	std::string goodbye_msg;

	if (msg.size() < 2)
		code = ERR_NEEDMOREPARAMS;
	else if (msg.size() > 2 && (msg[2][0] != ':' || msg[2].length() < 2))
		code = ERR_NEEDMOREPARAMS;
	else
		code = cmdCheck(client, channel, "");

	if (!code)
		code = channel->removeMember(*client);

	if (code)
		return (this->sendNumeric(*client, code));

	if (!channel->isEmpty())
		return (this->broadcast(*client, *channel, msg[0], this->unSplit(msg, 2)));

	this->_channels.erase(channel->getName());
	delete channel;
}

// Performs checks, sends name list & appropriate codes
void Server::cmdNames(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	if (msg.size() == 1)
	{
		channels_iter it = this->_channels.begin();
		for (; it != this->_channels.end(); ++it)
			this->cmdNames(client, it->second, this->newVector("NAMES", it->first, 0));
		return ;
	}
	else if (msg.size() != 2)
		code = ERR_NEEDMOREPARAMS;
	else
		code = cmdCheck(client, channel, "");

	if (code == ERR_NOTINCHAN)
		code = 0;

	if (code)
		return (this->sendNumeric(*client, code));

	this->sendNumeric(*client, RPL_NAMREPLY, this->nameList(*channel));
	this->sendNumeric(*client, RPL_ENDOFNAMES, ":End of /NAMES list.");
}

// Performs checks, sends message to relevant client/channel
void Server::cmdPrivmsg(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	if (msg.size() < 3 || (msg[2][0] != ':' || msg[2].size() < 2))
		code = ERR_NEEDMOREPARAMS;
	else
		code = cmdCheck(client, channel, msg[1]);

	if ((code == ERR_NOSUCHNICK && channel) || (code == ERR_NOSUCHCHAN && msg[1][0] != '#'))
		code = 0;

	if (!code && channel && !channel->isMember(*client))
		code = ERR_NOTINCHAN;

	if (code)
		return (this->sendNumeric(*client, code));

	std::string output = this->unSplit(msg, 2);

	if (channel)
		this->broadcast(*client, *channel, msg[0], output);
	else
		this->broadcast(*client, *this->_clients[msg[1]], msg[0], output);
}

// Performs checks, sends to channel topic method, sends relevant messages & codes
void Server::cmdTopic(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	if (!code && msg.size() < 5)
		code = ERR_NEEDMOREPARAMS;
	else if (!code && msg.size() > 2 && (msg[2][0] != ':' || msg[2].length() <= 1))
		code = ERR_UNKNOWNCOMMAND;
	else
		code = cmdCheck(client, channel, "");

	if (!code)
		code = channel->topicHandle(*client, msg);

	if (code == RPL_NOTOPIC)
		this->sendNumeric(*client, code, channel->getName() + " :No topic is set");
	else if (code == RPL_TOPIC)
		this->sendNumeric(*client, code, channel->getName() + " :" + channel->getTopic());
	else if (code)
		this->sendNumeric(*client, code);
	else
		this->broadcast(*client, *channel, msg[0], channel->getTopic());
}

// Performs checks, sends to flag dispatch function or sends back relevant messages & codes
void Server::cmdMode(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	code = cmdCheck(client, channel, "");
	int stop = checkModeFormat(msg);

	if (!code && stop == -1)
		code = ERR_UNKNOWNCOMMAND;

	if (code)
		return (this->sendNumeric(*client, code));

	if (msg.size() > 2)
		return (this->sendMode(*client, *channel, stop, msg));

	this->sendNumeric(*client, RPL_CHANMODE, channel->getName() + " " + channel->getModes());
	this->sendNumeric(*client, RPL_CREATTIME, channel->getName() + " " + channel->getCreat());
}

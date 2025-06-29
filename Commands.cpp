/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 12:48:21 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/29 18:39:08 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//------------------------------Commands---------------------------------------
#include "Server.hpp"

// Checks client existence, state & command format
void Server::cmdPass(Client *client, Channel *channel, str_vector const &msg)
{
	(void)channel;
	int code = 0;

	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->getState() >= PASS_OK)
		code = ERR_ALREADYAUTHED;
	else if (msg.size() != 2)
		code = ERR_NEEDMOREPARAMS;
	else if (msg[1] != this->_password)
		code = ERR_WRONGPASS;

	if (code)
		return (this->sendNumeric(*client, code));

	client->setState(PASS_OK);

	this->authCheck(*client);
}

// Checks client existence, state & command format
void Server::cmdNick(Client *client, Channel *channel, str_vector const &msg)
{
	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	(void)channel;
	int code = 0;

	if (msg.size() != 2)
		code = ERR_NEEDMOREPARAMS;
	else if (this->_clients.find(msg[1]) != this->_clients.end() || \
	client->getNickname() == msg[1])
		code = ERR_NICKINUSE;
	else if (msg[1].find_first_not_of(DIGIT_CHARS ALPHA_CHARS) != msg[1].npos)
		code = ERR_INVALIDNICK;

	if (code)
		return (this->sendNumeric(*client, code));

	if (client->getState() == ACTIVE)
		this->broadcast(*client, msg[0], msg[1]);

	client->setNickname(msg[1]);

	this->authCheck(*client);
}

// Checks client existence, state & command format
void Server::cmdUser(Client *client, Channel *channel, str_vector const &msg)
{
	(void)channel;
	int code = 0;

	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->getState() == ACTIVE || client->passedUser())
		code = ERR_ALREADYAUTHED;
	else if (msg.size() < 5)
		code = ERR_NEEDMOREPARAMS;
	else if (msg.size() > 5 && (msg[4][0] != ':' || msg[4].length() <= 1))
		code = ERR_UNKNOWNCOMMAND;

	if (code)
		return (this->sendNumeric(*client, code));

	client->setUsername(msg[1]);
	client->setRealname(this->unSplit(msg, 4));

	this->authCheck(*client);
}

// Checks client existence, state, command format & channel name format
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

	if (!channel)
	{
		channel = new Channel(msg[1]);
		this->_channels[msg[1]] = channel;
	}

	if (!code)
		code = channel->addMember(*client);

	if (code)
		return (this->sendNumeric(*client, code));

	this->cmdTopic(client, channel, this->newVector("TOPIC", channel->getName(), 0));
	this->cmdNames(client, channel, this->newVector("NAMES", channel->getName(), 0));

	this->broadcast(*client, *channel, msg[0], channel->getName());
}

void Server::cmdInvite(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;
	Client target = NULL;

	if (msg.size() != 3)
		code = ERR_NEEDMOREPARAMS;
	else
		code = cmdCheck(client, channel, msg[1]);

	if (!code)
		code = channel->setInvited(msg[1]);

	if (code)
		return (this->sendNumeric(*client, code));

	target = *this->_clients[msg[1]];

	this->sendNumeric(*client, RPL_INVITING, target.getNickname() + " " + channel->getName());
	this->broadcast(*client, target, msg[0], target.getNickname() + " :" + channel->getName());
}

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

void Server::cmdQuit(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	if (!client)
		throw std::runtime_error("Fatal: client not found");

	if (msg.size() > 1 && (msg[1][0] != ':' || msg[1].length() < 2))
		return (this->sendNumeric(*client, ERR_NEEDMOREPARAMS));

	channels_iter it = this->_channels.begin();
	while (it != this->_channels.end())
	{
		channel = it->second;
		it++;
		if (channel->isMember(*client))
			this->cmdPart(client, channel, this->newVector(msg[0], channel->getName(), &this->unSplit(msg, 1)));
	}

	this->removeClient(*client);
}

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
		code == ERR_NOTINCHAN;

	if (code)
		return (this->sendNumeric(*client, code));

	std::string output = this->unSplit(msg, 2);

	if (channel)
		this->broadcast(*client, *channel, msg[0], output);
	else
		this->broadcast(*client, *this->_clients[msg[1]], msg[0], output);
}

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


void Server::cmdMode(Client *client, Channel *channel, str_vector const &msg)
{
	int i = 0;
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


/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 12:48:21 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/28 17:47:24 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//------------------------------Commands---------------------------------------
#include "Server.hpp"

// Checks client existence, state & command format
void Server::cmdPass(Client *client, Channel *channel, std::vector<std::string> &msg)
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
		this->sendNumeric(*client, code);
	else
		client->setState(PASS_OK);

	this->authCheck(*client);
}

// Checks client existence, state & command format
void Server::cmdNick(Client *client, Channel *channel, std::vector<std::string> &msg)
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
void Server::cmdUser(Client *client, Channel *channel, std::vector<std::string> &msg)
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
		this->sendNumeric(*client, code);
	else
	{
		client->setUsername(msg[1]);
		client->setRealname(&msg[4][1]);
		for (size_t i = 5; i < msg.size(); i++)
			client->setRealname(client->getRealname() + " " + msg[i]);
	}

	this->authCheck(*client);
}

// Checks client existence, state, command format & channel name format
void Server::cmdJoin(Client *client, Channel *channel, std::vector<std::string> &msg)
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
		code = channel->addClient(*client);

	if (code)
		return (this->sendNumeric(*client, code));

	std::vector<std::string> topic_cmd = {{"TOPIC"}, {channel->getName()}};
	this->cmdTopic(client, channel, topic_cmd);

	std::string namelist = "= " + channel->getName() + " :";
	Channel::member_iter it = channel->getMembers().begin();
	for (; it != channel->getMembers().end(); ++it)
	{
		if (channel->isOperator((*it)->getNickname()))
			namelist += " @" + (*it)->getNickname();
		else
			namelist += " " + (*it)->getNickname();
	}

	this->sendNumeric(*client, RPL_NAMREPLY, namelist);
	this->sendNumeric(*client, RPL_ENDOFNAMES, ":End of /NAMES list.");
	this->broadcast(*client, *channel, msg[0], channel->getName());
}

// Creates channel if non-existing + adds client to channel
int Server::joinCmd(Client &client, Channel *channel, std::string channelname)
{
	if (!channel)
	{
		channel = new Channel(channelname);
		this->_channels[channelname] = channel;
	}

	return (channel->addClient(client));
}

void Server::privMsgCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{
	std::cout << "PRIVMSG function to be made" << std::endl;
	return (SUCCESS);
}

void Server::cmdTopic(Client *client, Channel *channel, std::vector<std::string> &msg)
{
	int code = 0;

	// Could I create a helper function that would centralize all checks?
	if (!client)
		throw (std::runtime_error("Fatal: client lost"));

	if (client->getState() != ACTIVE)
		code = ERR_NOTAUTHED;
	else if (!channel)
		code = ERR_NOSUCHCHAN;
	else if (!channel->isMember(*client))
		code = ERR_NOTINCHAN;
	else if (msg.size() < 5)
		code = ERR_NEEDMOREPARAMS;
	else if (split_msg.size() > 2 && (split_msg[2][0] != ':' || split_msg[2].length() <= 1))
		code = ERR_UNKNOWNCOMMAND;

	if (!code)
		code = channel->topicHandle(*client, split_msg);

	if (code == RPL_NOTOPIC)
		this->sendNumeric(*client, code, channel->getName() + " :No topic is set");
	else if (code == RPL_TOPIC)
		this->sendNumeric(*client, code, channel->getName() + " :" + channel->getTopic());
	else if (code)
		this->sendNumeric(*client, code);
	else
		this->broadcast(*client, *channel, split_msg[0], channel->getTopic());
}

void Server::inviteCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{}

void Server::privmsgCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{}

void Server::kickCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{}

void Server::modeCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{}

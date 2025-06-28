/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 12:48:21 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/28 16:33:59 by cbouvet          ###   ########.fr       */
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

	if (client->getState() == ACTIVE)
		code = ERR_ALREADYAUTHED;
	else if (msg.size() != 2)
		code = ERR_NEEDMOREPARAMS;
	else if (this->_clients.find(msg[1]) != this->_clients.end() || \
	client->getNickname() == msg[1])
		code = ERR_NICKINUSE;
	else if (msg[1].find_first_not_of(DIGIT_CHARS ALPHA_CHARS) != msg[1].npos)
		code = ERR_INVALIDNICK;

	if (code)
		this->sendNumeric(*client, code);
	else
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
	else if (msg.size() > 5 && (msg[4][0] != ':' || msg[24].length() <= 1))
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
void Server::joinCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{
	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->getState() <= AT_DOOR)
		return (RED "Error - Please enter the server using PASS" R);
	else if (client->getState() == PASS_OK)
		return (RED "Error - Please create a nickname using NICK" R);
	else if (client->getState() == NICK_OK)
		return (RED "Error - Please finalize user data using USER" R);

	if (split_msg.size() != 2)
		return (RED "JOIN: invalid command format\n" JOIN_EXPECT);

	if (split_msg[1][0] != '#' || split_msg[1].length() < 2)
		return (RED "Invalid channel name format\n" JOIN_EXPECT);

	return (this->joinCmd(*client, channel, split_msg[1]));
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

void Server::topicCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{
	//See if can use ref instead
	if (!client)
		throw (std::runtime_error("Fatal: client lost"));

	int code = 0; // Could I create a helper function that would centralize all checks?

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

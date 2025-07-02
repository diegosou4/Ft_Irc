/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AuthCmds.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 21:25:25 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/02 13:38:38 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//------------------------------AuthCmds---------------------------------------
#include "Server.hpp"

void Server::cmdPong(Client *client, Channel *channel, str_vector const &msg)
{
	(void)channel;

	if (!client || msg.size() < 2)
		return ;

	if (msg[0] == this->_name)
		client->setPinged(false);

	client->setLastActivity();
}

// Performs checks, switches client state to PASS_OK if pass is valid
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

// Performs checks, updates nickname in client's channels + _client maps, broadcasts message
void Server::cmdNick(Client *client, Channel *channel, str_vector const &msg)
{
	(void)channel;
	int code = 0;

	if (!client)
		throw (std::runtime_error("Fatal: client not found"));
	if (msg.size() != 2)
		code = ERR_NEEDMOREPARAMS;
	else if (this->_clients.find(msg[1]) != this->_clients.end() || \
	client->getNickname() == msg[1])
		code = ERR_NICKINUSE;
	else if (msg[1].find_first_not_of(DIGIT_CHARS ALPHA_CHARS) != msg[1].npos)
		code = ERR_INVALIDNICK;

	if (code)
		return (this->sendNumeric(*client, code));

	for (channels_iter it = this->_channels.begin(); it != this->_channels.end(); ++it)
	{
		it->second->updateNickname(client->getNickname(), msg[1]); // done even if user is non member, because can be on invited list
		if (it->second->isMember(*client))
			this->broadcast(*client, *it->second, msg[0], msg[1]);
	}

	this->_clients.erase(client->getNickname());
	this->_clients[msg[1]] = client;
	client->setNickname(msg[1]);

	this->authCheck(*client);
}

// Performs checks, updates username & realname
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
	client->setRealname(this->argExists(msg, 4));

	this->authCheck(*client);
}

// Performs checks, sends to PART for all channels that client was in, sends to removal
void Server::cmdQuit(Client *client, Channel *channel, str_vector const &msg)
{
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
		{
			std::string goodbye_msg = this->argExists(msg, 1);
			this->cmdPart(client, channel, this->newVector(msg[0], channel->getName(), &goodbye_msg));
		}
	}

	this->removeClient(client);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AuthCmds.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 21:25:25 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/06 13:13:52 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//------------------------------AuthCmds---------------------------------------
#include "../includes/Server.hpp"

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
		return (this->sendNumeric(*client, 0, code));

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
		code = ERR_NONICK;
	else if (this->_clients.find(msg[1]) != this->_clients.end() || \
	client->getNickname() == msg[1])
		code = ERR_NICKINUSE;
	else if (this->parseNick(msg[1]) != SUCCESS)
		code = ERR_INVALIDNICK;

	if (code)
		return (this->sendNumeric(*client, 0, code));

	std::string oldname = client->getNickname();

	this->broadcast(*client, *client, msg[0] , msg[1]);
	
	client->setNickname(msg[1]);
	client->setPrefix();
	this->_clients.erase(oldname);
	this->_clients[msg[1]] = client;

	for (channels_iter it = this->_channels.begin(); it != this->_channels.end(); ++it)
	{
		it->second->updateNickname(oldname, msg[1]);
		if (it->second->isMember(*client))
			this->broadcastOthers(*client, *it->second, msg[0] , msg[1]);
	}

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
		return (this->sendNumeric(*client, 0, code));

	client->setUsername(msg[1]);
	client->setRealname(this->argExists(msg, 4));

	this->authCheck(*client);
}

// Performs checks, sends to PART for all channels that client was in, sends to removal
void Server::cmdQuit(Client *client, Channel *channel, str_vector const &msg)
{
	(void)channel;

	if (!client)
		throw std::runtime_error("Fatal: client not found");

	if (msg.size() > 1 && (msg[1][0] != ':' || msg[1].length() < 2))
		return (this->sendNumeric(*client, 0, ERR_NEEDMOREPARAMS));

	this->leaveAllChans(msg[0], *client, this->argExists(msg, 1));
	this->removeClient(client);
}

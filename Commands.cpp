/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 12:48:21 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/27 10:51:43 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//------------------------------Commands---------------------------------------
#include "Server.hpp"

// Checks client existence, state & command format
std::string Server::passCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{
	(void)channel;

	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->getState() > AT_DOOR)
		return ("You are already logged into the server");

	if (split_msg.size() != 2)
		return ("PASS: invalid command format\n" PASS_EXPECT);

	return (this->passCmd(*client, split_msg[1]));
}

// Checks client existence, state & command format
std::string Server::nickCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{
	(void)channel;

	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->getState() < PASS_OK)
		return ("Error - " INSTRUCTIONS R);
	else if (client->getState() > PASS_OK)
		return ("Your nickname has already been set");

	if (split_msg.size() != 2)
		return ("NICK: invalid command format\n" NICK_EXPECT);

	return (this->nickCmd(client, split_msg[1]));
}

// Checks client existence, state & command format
std::string Server::userCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{
	(void)channel;

	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->getState() <= AT_DOOR)
		return (RED "Error - Please enter the server using PASS" R);
	else if (client->getState() == PASS_OK)
		return (RED "Error - Please create a nickname using NICK" R);

	if (split_msg.size() < 5 || split_msg[4][0] != ':' || split_msg[4].size() <= 1)
		return (RED "USER: invalid command format\n" USER_EXPECT);

	return (this->userCmd(*client, split_msg));
}

// Checks client existence, state, command format & channel name format
std::string Server::joinCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg)
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

// Checks password + sets client to next state if correct
std::string Server::passCmd(Client &client, std::string password)
{
	if (password != this->_password)
	{
		this->printServer(&client, "access denied - invalid password");
		return (RED "Invalid password - access denied" R);
	}

	client.setState(PASS_OK);

	this->printServer(&client, "access granted");
	return ("Password is correct - access granted!\nPlease proceed with NICK");
}

// Sets existing client as active OR sets non-existing client to next state
std::string Server::nickCmd(Client *client, std::string nickname)
{
	if (!client)
		return (NULL);

	if (this->_clients.find(nickname) == this->_clients.end())
	{
		this->printServer(client, "set nickname to " + nickname);

		client->setNickname(nickname);
		client->setState(NICK_OK);

		return ("Nickname created - Please proceed with USER");
	}

	this->_clients.erase(client->getNickname());
	delete client;

	client = this->_clients[nickname];
	client->setState(ACTIVE);

	this->printServer(client, "successfully logged in");
	return ("Welcome back, " + client->getNickname());
}

// Updates username and realname + finalizes new client registration if applicable
std::string Server::userCmd(Client &client, std::vector<std::string> &user_args)
{
	client.setUsername(user_args[0]);

	client.setRealname(&user_args[4][1]);
	for (size_t i = 5; i < user_args.size(); i++)
		client.setRealname(client.getRealname() + " " + user_args[i]);

	this->printServer(&client, "changed user data to:\n" GREY " > username: " R + \
		client.getUsername() + GREY "	-	realname: " R + client.getRealname());

	if (!client.getUsername().empty() && client.getState() == ACTIVE)
		return ("Your user data has been correctly updated");

	client.setState(ACTIVE);
	return ("Welcome, " + client.getUsername());
}

// Creates channel if non-existing + adds client to channel
std::string Server::joinCmd(Client &client, Channel *channel, std::string channelname)
{
	if (!channel)
	{
		channel = new Channel(channelname);
		this->_channels[channelname] = channel;
	}

	return (channel->addClient(client));
}

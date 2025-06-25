/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 12:48:21 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/20 12:59:11 by cbouvet          ###   ########.fr       */
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

	if (client->state > AT_DOOR)
		return (PURPLE "You are already logged into the server" R);

	if (split_msg.size() != 2)
		return (RED "PASS: invalid command format\n" GREY PASS_EXPECT R);

	return (this->passCmd(*client, split_msg[1]));
}

// Checks client existence, state & command format
std::string Server::nickCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{
	(void)channel;

	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->state < PASS_OK)
		return (RED "Error - " INSTRUCTIONS R);
	else if (client->state > PASS_OK)
		return (PURPLE "Your nickname has already been set" R);

	if (split_msg.size() != 2)
		return (RED "NICK: invalid command format\n" GREY NICK_EXPECT R);

	return (this->nickCmd(client, split_msg[1]));
}

// Checks client existence, state & command format
std::string Server::userCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{
	(void)channel;

	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->state <= AT_DOOR)
		return (RED "Error - Please enter the server using PASS" R);
	else if (client->state == PASS_OK)
		return (RED "Error - Please create a nickname using NICK" R);

	if (split_msg.size() < 5 || split_msg[4][0] != ':' || split_msg[4].size() <= 1)
		return (RED "USER: invalid command format\n" GREY USER_EXPECT R);

	return (this->userCmd(*client, split_msg));
}

// Checks client existence, state, command format & channel name format
std::string Server::joinCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg)
{
	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->state <= AT_DOOR)
		return (RED "Error - Please enter the server using PASS" R);
	else if (client->state == PASS_OK)
		return (RED "Error - Please create a nickname using NICK" R);
	else if (client->state == NICK_OK)
		return (RED "Error - Please finalize user data using USER" R);

	if (split_msg.size() != 2)
		return (RED "JOIN: invalid command format\n" GREY JOIN_EXPECT R);

	if (split_msg[1][0] != '#' || split_msg[1].length() < 2)
		return (RED "Invalid channel name format\n" GREY JOIN_EXPECT R);

	return (this->joinCmd(*client, channel, split_msg[1]));
}

// Checks password + sets client to next state if correct
std::string Server::passCmd(Client &client, std::string password)
{
	if (password != this->_password)
	{
		std::cout << PURPLE << client.nickname << R " access denied: invalid password" << std::endl;
		return (RED "Invalid password - access denied" R);
	}

	client.state = PASS_OK;

	std::cout << PURPLE << client.nickname << R " access granted" << std::endl;
	return (GREY "Password is correct - access granted!\nPlease proceed with NICK" R);
}

// Sets existing client as active OR sets non-existing client to next state
std::string Server::nickCmd(Client *client, std::string nickname)
{
	if (this->_clients.find(nickname) == this->_clients.end())
	{
		std::cout << PURPLE << client->nickname << R " set nickname to " GREY << nickname << R << std::endl;

		client->nickname = nickname;
		client->state = NICK_OK;

		return (GREY "Nickname created - Please proceed with USER" R);
	}

	this->_clients.erase(client->nickname);
	delete client;

	client = this->_clients[nickname];
	client->state = ACTIVE;

	std::cout << PURPLE << client->nickname << R " logged in" << std::endl;
	return (PURPLE "Welcome back, " + client->nickname + R);
}

// Updates username and realname + finalizes new client registration if applicable
std::string Server::userCmd(Client &client, std::vector<std::string> &user_args)
{
	client.username = user_args[0];

	client.realname = &user_args[4][1];
	for (size_t i = 5; i < user_args.size(); i++)
		client.realname += " " + user_args[i];

	std::cout << PURPLE << client.nickname << R " changed user data to:\n"
	<< GREY " > username: " R << client.username << GREY "	-	realname: " R << client.realname << std::endl;

	if (!client.username.empty() && client.state == NICK_OK)
		return (PURPLE "Your user data has been correctly updated" R);

	client.state = ACTIVE;
	return (PURPLE "Welcome, " + client.nickname + R);
}

// Creates channel if non-existing + adds client to channel
std::string Server::joinCmd(Client &client, Channel *channel, std::string channelname)
{
	if (!channel)
	{
		channel = new Channel(channelname);
		this->_channels[channelname] = channel;
	}

	return (channel->joinCmd(client));
}

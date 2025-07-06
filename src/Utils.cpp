/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 13:00:56 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/06 10:17:11 by feden-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//--------------------------------Utils----------------------------------------
#include "../includes/Server.hpp"

// Creates maps linking command to an function
void	Server::setCmdMaps()
{
	this->_authcmds["PASS"] = &Server::cmdPass;
	this->_authcmds["NICK"] = &Server::cmdNick;
	this->_authcmds["USER"] = &Server::cmdUser;
	this->_authcmds["JOIN"] = &Server::cmdJoin;
	this->_authcmds["INVITE"] = &Server::cmdInvite;
	this->_authcmds["KICK"] = &Server::cmdKick;
	this->_authcmds["PART"] = &Server::cmdPart;
	this->_authcmds["QUIT"] = &Server::cmdQuit;
	this->_authcmds["NAMES"] = &Server::cmdNames;
	this->_authcmds["PRIVMSG"] = &Server::cmdPrivmsg;
	this->_authcmds["NOTICE"] = &Server::cmdNotice;
	this->_authcmds["TOPIC"] = &Server::cmdTopic;
	this->_authcmds["MODE"] = &Server::cmdMode;
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
		this->cmdQuit(&client, NULL, newVector("QUIT", ":Dropped unexpectedly", 0));
		buff[0] = 0;
	}

	return (std::string(buff));
}

// Splits user command into vector string items
Server::str_vector Server::splitMsg(std::string &msg)
{
	std::string str;
	str_vector split_msg;
	std::stringstream ss(msg);

	while (ss >> str)
	{
		if (str[0] == ':')
		{
			std::string arg;
			std::getline(ss, arg);
			if (!arg.empty())
				str += arg;
			split_msg.push_back(str);
			break;
		}
		split_msg.push_back(str);
	}

	return (split_msg);
}

// Removes client from pollfd + _clients & deletes it
void	Server::removeClient(Client *client)
{
	for (pollfd_iter it = this->_fds.begin(); it != this->_fds.end(); ++it)
		if (client->getFd() == it->fd)
			it->fd = REMOVAL;

	this->printServer(client, "has left");

	this->_clients.erase(client->getNickname());
	close(client->getFd());
	delete client;
	client = NULL;
}

// Checks if client has passed PASS, NICK and USER to be considered ACTIVE
bool	Server::authCheck(Client &client)
{
	if (client.getState() == ACTIVE)
		return (true);

	if (client.getState() != PASS_OK || !client.passedNick() || !client.passedUser())
		return (false);

	client.setState(ACTIVE);
	client.setPrefix();

	this->sendNumeric(client, RPL_WELCOME, WELCOME);
	this->printServer(&client, "has successfully logged in");

	return (true);
}

// Creates vector from 2 or 3 strings
Server::str_vector Server::newVector(std::string const &arg1, std::string const &arg2, std::string *arg3)
{
	str_vector new_vector;

	new_vector.push_back(arg1);
	new_vector.push_back(arg2);
	if (arg3)
		new_vector.push_back(*arg3);

	return (new_vector);
}

// Joins vector indexes into a space separated string
std::string	Server::argExists(str_vector const &msg, size_t index)
{
	if (index >= msg.size())
		return ("");

	if(msg[index][0] == ':')
	{
		size_t start = msg[index].find_first_not_of(':');
		return (msg[index].substr(start));
	}

	return (msg[index]);
}

int Server::parseNick(const std::string &nick)  // We need this function to check if the nickname is valid
{
	if (nick.empty())
		return ERR_INVALIDNICK;

	if (!std::isalpha(static_cast<unsigned char>(nick[0])))
		return ERR_INVALIDNICK;

	for (size_t i = 1; i < nick.length(); ++i)
	{
		char c = nick[i];
		if (!std::isalnum(static_cast<unsigned char>(c)) &&
		    std::string("[]\\`_^{|}-").find(c) == std::string::npos)
			return ERR_INVALIDNICK;
	}

	return SUCCESS;
}

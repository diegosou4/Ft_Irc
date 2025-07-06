/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MsgSenders.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 21:47:44 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/06 13:15:33 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//--------------------------------MsgSenders----------------------------------------
#include "../includes/Server.hpp"

// Composes message + prints on server standard output
void	Server::printServer(Client *client, std::string const &msg)
{
	if (client)
		std::cout << PURPLE << client->getNickname()<< ": " R;

	std::cout << msg << R << std::endl;
}

// Sends given message to given client
void	Server::sendClient(Client &client, std::string const &msg)
{
	try
	{
		if (send(client.getFd(), msg.c_str(), msg.length(), 0) < 0)
			this->printServer(&client, RED "Failed to send message");
	}
	catch (const std::exception &e)
	{
		std::cerr << "Error sending message to client: " << e.what() << std::endl;
	}
}

// Composes message based on provided code, sends to client
void	Server::sendNumeric(Client &client, Channel *channel, int code)
{
	std::stringstream ss;
	std::string output;

	if (ErrMsg.find(code) == ErrMsg.end())
		throw (std::runtime_error("Invalid error code"));

	if (channel)
		output = channel->getName() + " ";
	output += ErrMsg.find(code)->second;

	ss 	<< ":" << this->_name << " " \
		<< std::setw(3) << std::setfill('0') \
		<< code << " " << client.getNickname() \
		<< " " << output << "\r\n";

	this->sendClient(client, ss.str());
}

// Sends code and message to client
void	Server::sendNumeric(Client &client, int code, std::string const &msg)
{
	std::stringstream ss;

	ss 	<< ":" << this->_name << " " \
		<< std::setw(3) << std::setfill('0') \
		<< code << " " << client.getNickname() \
		<< " " << msg << "\r\n";

	this->sendClient(client, ss.str());
}

// Composes message, sends to given target client
void	Server::broadcast(Client &client, Client &target, std::string const &cmd, std::string const &msg)
{
	std::string output = client.getPrefix() + " " + cmd + " " + msg + "\r\n";

	if (target.getState() == ACTIVE)
		this->sendClient(target, output);
}

// Composes message, sends to all channels where client is a member
void	Server::broadcastAll(Client &client, Channel &channel, std::string const &cmd, std::string const &msg)
{
	std::string output = client.getPrefix() + " " + cmd + " :" +  msg +  "\r\n";
	if (cmd == "MODE" || cmd == "KICK" || cmd == "TOPIC" || cmd == "PART" || cmd == "QUIT")
		output = client.getPrefix() + " " + cmd + " " + msg + "\r\n";

	std::cout << output << std::endl;
	Channel::member_iter it = channel.getMembers().begin();
	for (; it != channel.getMembers().end(); ++it)
		if ((*it)->getState() == ACTIVE)
			this->sendClient(**it, output);
}

void	Server::broadcastOthers(Client &client, Channel &channel, std::string const &cmd, std::string const &msg)
{
	std::string output = client.getPrefix() + " " + cmd + " :" +  msg +  "\r\n";
	if (cmd == "PRIVMSG" && channel.isOperator(client.getNickname()))
		output = "@" + output;
	if (cmd == "PRIVMSG")
		output = client.getPrefix() + " " + cmd + " " + msg + "\r\n";

	Channel::member_iter it = channel.getMembers().begin();
	for (; it != channel.getMembers().end(); ++it)
		if ((*it) != &client && (*it)->getState() == ACTIVE)
			this->sendClient(**it, output);
}



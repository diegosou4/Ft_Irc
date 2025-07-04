/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MsgSenders.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 21:47:44 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/02 23:27:23 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//--------------------------------MsgSenders----------------------------------------
#include "Server.hpp"

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
	if (send(client.getFd(), msg.c_str(), msg.length(), 0) < 0)
		this->printServer(&client, RED "Failed to send message");
}

// Composes message based on provided code, sends to client
void	Server::sendNumeric(Client &client, int code)
{
	std::stringstream ss;
	std::string msg;

	if (ErrMsg.find(code) == ErrMsg.end())
		throw (std::runtime_error("Invalid error code"));

	msg = ErrMsg.find(code)->second;

	ss 	<< ":" << this->_name << " " \
		<< std::setw(3) << std::setfill('0') \
		<< code << " " << client.getNickname() \
		<< " " << msg << "\r\n";

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

void	Server::sendNumeric(Client &client, Channel *channel, int code)
{
	std::stringstream ss;
	std::string msg;

	if (ErrMsg.find(code) == ErrMsg.end())
		throw (std::runtime_error("Invalid error code"));

	msg = ErrMsg.find(code)->second;
	if (channel)
		msg = channel->getName() + " " + msg;

	ss 	<< ":" << this->_name << " " \
		<< std::setw(3) << std::setfill('0') \
		<< code << " " << client.getNickname() \
		<< " " << msg << "\r\n";

	this->sendClient(client, ss.str());
}

void	Server::broadcast(Client &client, Client &target, std::string const &cmd, std::string const &msg)
{
	std::string output = client.getPrefix() + " " + cmd + " " + msg + "\r\n";
	if (target.getState() == ACTIVE)
		this->sendClient(target, output);
}


// Changed By Diego
void Server::broadcastAll(Client &client, Channel &channel, const std::string &cmd, const std::string &msg)
{
	std::string output = client.getPrefix() + " " + cmd + " " + channel.getName() + " " + msg + "\r\n";

	for (Channel::member_iter it = channel.getMembers().begin(); it != channel.getMembers().end(); ++it)
	{
		if ((*it)->getState() == ACTIVE)
			this->sendClient(**it, output); 
	}
}


// Composes message, sends to all channels where client is a member
void	Server::broadcast(Client &client, Channel &channel, std::string const &cmd, std::string const &msg)
{
	std::string output = client.getPrefix() + " " + cmd + " " + channel.getName() + " " + msg + "\r\n";
	if (channel.isOperator(client.getNickname()))
		output = "@" + output;

	Channel::member_iter it = channel.getMembers().begin();
	for (; it != channel.getMembers().end(); ++it)
		if ((*it) != &client && (*it)->getState() == ACTIVE)
			this->sendClient(**it, output);
}


// Changed By Diego
void	Server::broadcastJoin(Client &client, Channel &channel)
{
	std::string output = client.getPrefix() + " JOIN :" + channel.getName() + "\r\n";
	
	// if (channel.isOperator(client.getNickname()))
	// 	output = "@" + output;
	
	Channel::member_iter it = channel.getMembers().begin();
	for (; it != channel.getMembers().end(); ++it)
		if ((*it)->getState() == ACTIVE)
			this->sendClient(**it, output);
}



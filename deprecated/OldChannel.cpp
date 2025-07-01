/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   OldChannel.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 20:47:34 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/27 11:23:09 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "OldChannel.hpp"

OldChannel::OldChannel(std::string name): _name(name)
{}

OldChannel::~OldChannel()
{}

std::string OldChannel::joinCmd(Client &client)
{
	std::find(this->members.begin(), this->members.end(), &client) != this->members.end();
	if (std::find(this->members.begin(), this->members.end(), &client) != this->members.end())
		return (client.getUsername() + " is already in channel " + this->_name);

	this->members.push_back(&client);
	return (client.getUsername() + " joined " + this->_name);
}

std::string OldChannel::modeCmd(Client &client, std::vector<std::string> &msg)
{
	return (client.getUsername() + " change channel " + this->_name + " mode to " + msg[2]);
}

std::string OldChannel::topicCmd(Client &client, std::vector<std::string> &msg)
{
	return (client.getUsername() + " changed " + this->_name + " topic to " + &msg[2][1]);
}

std::string OldChannel::inviteCmd(Client &client, std::vector<std::string> &msg)
{
	return (client.getUsername() + " invited " + msg[1] + " to " + this->_name);
}

std::string OldChannel::privmsgCmd(Client &client, std::vector<std::string> &msg)
{
	return (client.getUsername() + this->_name + ": " + &msg[2][1]);
}

std::string OldChannel::kickCmd(Client &client, std::vector<std::string> &msg)
{
	return (client.getUsername() + " kicked " + msg[2] + " from " + this->_name + " due to: " + &msg[3][1]);
}

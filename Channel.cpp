/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 20:47:34 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/25 19:00:11 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

Channel::Channel(std::string name): _name(name)
{}

Channel::~Channel()
{}

std::string Channel::joinCmd(Client &client)
{
	std::find(this->members.begin(), this->members.end(), &client) != this->members.end();
	if (std::find(this->members.begin(), this->members.end(), &client) != this->members.end())
		return (RED + client.getUsername() + R " is already in channel " + this->_name);

	this->members.push_back(&client);
	return (PURPLE + client.getUsername() + R " joined " + this->_name);
}

std::string Channel::modeCmd(Client &client, std::vector<std::string> &msg)
{
	return (PURPLE + client.getUsername() + R " change channel " + this->_name + " mode to " + msg[2]);
}

std::string Channel::topicCmd(Client &client, std::vector<std::string> &msg)
{
	return (PURPLE + client.getUsername() + R " changed " + this->_name + " topic to " + &msg[2][1]);
}

std::string Channel::inviteCmd(Client &client, std::vector<std::string> &msg)
{
	return (PURPLE + client.getUsername() + R " invited " + msg[1] + " to " + this->_name);
}

std::string Channel::privmsgCmd(Client &client, std::vector<std::string> &msg)
{
	return (PURPLE + client.getUsername() + R + this->_name + ": " + &msg[2][1]);
}

std::string Channel::kickCmd(Client &client, std::vector<std::string> &msg)
{
	return (PURPLE + client.getUsername() + R " kicked " + msg[2] + " from " + this->_name + " due to: " + &msg[3][1]);
}

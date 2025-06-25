/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 20:54:27 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/25 22:07:04 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

//----------------- Constructor/Destructor ------------------
Channel::Channel(std::string const &name): _name(name), _limit(50)
{}

Channel::Channel(std::string const &name, int const limit): _name, _limit(limit)
{}

Channel::~Channel()
{
	// Possibly clear memory of containers here?
}

//------------------------ Setters --------------------------
void	Channel::setName(std::string const &name)
{
	this->_name = name;
}

void	Channel::setTopic(std::string const &topic) // checks to be handled in topicCmd
{
	this->_topic = topic;
}

void	Channel::setPassword(std::string const &password) // checks to be handled in modeCmd
{
	this->_password = password;
}

void	Channel::setLimit(int const &limit)
{
	this->_limit = limit;
}

void	Channel::setOperator(Client const &target)
{
	this->_operators.insert(target.getNickname());
	/* previous notation:
	 _operators.insert(_operators.end(), client->getNickname());
	doesn't work:
	Even if trying to add at end of container (which doesn't work here),
	sets always store elements in ascii order.
	Is order really important here?
	Should we use a different type of container? */
}

void	Channel::setBanned(Client const &target)
{
	this->_banned.insert(target.getNickname());
}


//------------------------ Getters---------------------------
std::string	Channel::getName() const
{}

std::string	Channel::getTopic() const
{}

std::vector<Client *> Channel::getMembers() const
{}


//-------------------- Command methods-----------------------
std::string Channel::joinCmd(Client &client)
{}

std::string Channel::modeCmd(Client &client, std::vector<std::string> &msg)
{
	if (!this->isMember(client))
		return (RED " is not a member of " + this->_name);

	if (this->_operators.find(client.getNickname()) == this->_operators.end())
		return (RED " is not an operator of " + this->_name);

	// decompose message
	// switch flags
	// send to appropriate action
}

std::string Channel::topicCmd(Client &client, std::vector<std::string> &msg)
{}

std::string Channel::inviteCmd(Client &client, std::vector<std::string> &msg)
{}

std::string Channel::privmsgCmd(Client &client, std::vector<std::string> &msg)
{}

std::string Channel::kickCmd(Client &client, std::vector<std::string> &msg)
{}


//------------------------- Utils----------------------------
bool	Channel::isEmpty() const
{}

bool	Channel::isMember(Client &client) const
{}

bool	Channel::isOperator(std::string nick) const
{}

void	Channel::unban(Client *op, Client *target)
{}

std::string	Channel::addClient(Client &client, std::vector<std::string> &msg)
{}


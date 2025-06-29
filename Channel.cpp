/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 20:54:27 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/29 17:01:56 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

//----------------- Constructor/Destructor ------------------
Channel::Channel(std::string const &name): _name(name), _limit(50)
{}

Channel::Channel(std::string const &name, int const limit): _name(name), _limit(limit)
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

void	Channel::setModes(char sign, char flag)
{
	/* this method must be called everytime a channel mode is altered
	if char is +, check if flag already in modes
		if not, adds it
	if char is -, check if flag is already non-existing
		if exists, removes it */
}

bool	Channel::setOperator(std::string target)
{
	if (this->isOperator(target))
		return (false);

	this->_operators.insert(target);
	return (true);
	/* previous notation:
	 _operators.insert(_operators.end(), client->getNickname());
	doesn't work:
	Even if trying to add at end of container (which doesn't work here),
	sets always store elements in ascii order.
	Is order really important here?
	Should we use a different type of container? */
}

int Channel::setInvited(std::string const &name)
{
	/* if already member
		return ERR_USERINCHAN
	else return SUCCESS*/

	std::cout << "INVITED Channel method WIP" << std::endl;
	return (SUCCESS);
}

//------------------------ Getters---------------------------
std::string	Channel::getName() const
{
	return (this->_name);
}

std::string	Channel::getTopic() const
{
	return (this->_topic);
}

std::string Channel::getModes() const
{
	return (this->_modes);
}

std::string Channel::getCreat() const
{
	return (this->_creat);
}

std::vector<Client *> Channel::getMembers() const
{
	return (this->_members);
}


//-------------------- Command-related methods-----------------------
int	Channel::addMember(Client &client)
{
	/* Perform checks
		if channel is at capacity
			return ERR_CHANISFULL
		if channel is invite only && !invited
			return ERR_INVITEONLYCHAN
		if user already in
			return ERR_USERINCHAN
	add user to channel member
	return SUCCESS*/

	std::cout << "TOPIC Channel method WIP" << std::endl;
	return (SUCCESS);
}

int Channel::kickMember(Client &client, std::string const &target)
{
	/*
		if client not op
			return ERR_NOTCHANOP
	if target not a member
		 return ERR_USERNOTINCHAN
	call rmMember
	remove user from all relevant containers
	return SUCCESS
		 */

	std::cout << "KICK Channel method WIP" << std::endl;
	return (SUCCESS);
}

int	Channel::removeMember(Client &client)
{
	/* remove user from all relevant containers
	return SUCCESS */

	std::cout << "REMOVE Channel method WIP" << std::endl;
	return (SUCCESS);
}

int Channel::topicHandle(Client &client, std::vector<std::string> const &msg)
{
	/* differentiate between:
		get topic
			return RPL_NOTOPIC if none
			return RPL_TOPIC if exists

		set topic
			here, legitimacy of client must be verified
				return ERR_NOTCHANOP if client is not op
			if change successful ->
			recompose message
			this->setTopic
			return SUCCESS*/

	std::cout << "TOPIC Channel method WIP" << std::endl;
	return (SUCCESS);
}

int	Channel::modeFlags(Client &client, char sign, char flag, std::string arg)
{
	/* check if needsArg(sign, flag) == true && arg.empty()
			return ERR_NEEDMOREPARAMS
		else if !(needsArg(sign, flag) && !arg.empty()
			return ERR_UNKNOWNCOMMAND

		dispatch to appropriate functions according to sign + flag
		eg.
		switch or if else
			flag == i && sign == +
				send to channel function handling +i
					return appropriate error code if any

	if all goes well, return SUCCCESS*/

	std::cout << "modeFlags method WIP" << std::endl;
	return (SUCCESS);
}

//------------------------- Utils----------------------------
bool	Channel::isEmpty() const
{
	return (this->_members.empty());
}

bool	Channel::isMember(Client &client) const
{
	/* check if client is a member */
	std::cout << "isMember method WIP" << std::endl;
	return (true);
}

bool	Channel::isOperator(std::string nick) const
{
	if (this->_operators.find(nick) != this->_operators.end())
		return (true);

	return (false);
}

bool	Channel::needsArg(char sign, char flag)
{
	/* check which sign + flag require arg
		if do
			return true
		return false */

	std::cout << "NeedsArg method WIP" << std::endl;
	return (false);
}

Client *Channel::findMember(std::string name)
{
	member_iter it = this->_members.begin();

	for (; it != this->_members.end(); ++it)
	{
		if ((*it)->getNickname() == name)
			return (*it);
	}

	return (NULL);
}


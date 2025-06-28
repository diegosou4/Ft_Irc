/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 20:54:27 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/28 19:49:47 by cbouvet          ###   ########.fr       */
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

bool	Channel::setBanned(std::string target) //Ban is not compulsory in subject
{
	if (this->_banned.find(target) != this->_banned.end())
		return (false);

	member_iter it = this->_members.begin();
	for (; it != this->_members.end(); ++it)
		if ((*it)->getNickname() == target)
			break;

	if (it != this->_members.end())
		this->_members.erase(it);

	if (this->isOperator(target))
		this->_operators.erase(target);

	this->_banned.insert(target);

	return (true);
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

std::vector<Client *> Channel::getMembers() const
{
	return (this->_members);
}


//-------------------- Command methods-----------------------
int Channel::modeCmd(Client &client, std::vector<std::string> &msg)
{
	if (!this->isMember(client))
		return (ERR_NOTINCHAN);

	if (!isOperator(client.getNickname()))
		return (ERR_NOTCHANOP);

	//perform format checks

	return (this->modeFlags(client, msg[1][0], msg[2]));
	// COULD DO A HELPER WITH NOTHING + ANOTHER WITH INT + ANOTHER WITH STR +ANOTHER W CLIENT
}

int Channel::topicHandle(Client &client, std::vector<std::string> &msg)
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

int Channel::inviteCmd(Client &client, std::vector<std::string> &msg)
{
	(void)client;
	(void)msg;
	return ("invite function to be made");
}

int Channel::kickCmd(Client &client, std::vector<std::string> &msg)
{
	(void)client;
	(void)msg;
	return ("kick function to be made");
}


//------------------------- Utils----------------------------
bool	Channel::isEmpty() const
{
	return (this->_members.empty());
}

bool	Channel::isMember(Client &client) const
{
	(void)client;
	return (true);
}

bool	Channel::isOperator(std::string nick) const
{
	if (this->_operators.find(nick) != this->_operators.end())
		return (true);

	return (false);
}

void	Channel::unban(Client *op, Client *target)
{
	(void)op;
	(void)target;
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

int	Channel::addClient(Client &client)
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

// Need to workout +/- differences for flags
int Channel::modeFlags(Client &client, char flag, std::string arg)
{
	(void)client;

	if (flag == 'i' || flag == 't')
	{
		if (!arg.empty())
			return ("Invalid use of flag " + flag);
		if (flag == 'i')
			std::cout << "invite function to be made";
		else if (flag == 't')
			std::cout << "topic function to be made";
	}

	if (arg.empty())
			return ("Invalid use of flag " + flag);

	if (flag == 'k')
	{
		this->setPassword(arg);
		return (" has set password to " + arg);
	}
	else if (flag == 'l')
	{
		if (arg.find_first_not_of(DIGIT_CHARS))
			return ("Invalid limit: has to be numerical");

		int limit = atoi(arg.c_str());
		if (arg.length() > 2 || limit > 50)
			return ("Invalid limit: can't be higher than 50");

		this->setLimit(limit);
		return (" has set channel member limit to " + arg);
	}

	if (!this->findMember(arg))
			return (arg + " is not a member of " + this->_name);

	if (flag == 'o')
	{
		if (!this->setOperator(arg))
			return (arg + "is already an operator of " + this->_name);
		return (" has set " + arg + " as operator of " + this->_name);
	}
	else if (flag == 'b')
	{
		if (!this->setBanned(arg))
			return (arg + " is already banned from " + this->_name);
		return (" has banned " + arg + " from " + this->_name);
	}
	return ("unsure yet");
}

int Channel::setInvited(std::string &name)
{
	/* if already member
		return ERR_USERINCHAN
	else return SUCCESS*/

	std::cout << "INVITED Channel method WIP" << std::endl;
	return (SUCCESS);
}

int Channel::kickMember(Client &client, std::string &target)
{
	/*
		if client not op
			return ERR_NOTCHANOP
	if target not a member
		 return ERR_USERNOTINCHAN
	remove user from all relevant containers
	return SUCCESS
		 */

	std::cout << "KICK Channel method WIP" << std::endl;
	return (SUCCESS);
}


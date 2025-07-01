/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 20:54:27 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/01 18:20:53 by cbouvet          ###   ########.fr       */
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

void	Channel::setKey(std::string const &key) // checks to be handled in modeCmd
{
	this->_key = key;
}

void	Channel::setLimit(int const &limit)
{
	this->_limit = limit;
}

void	Channel::setModes(char sign, char flag)
{
	(void)sign;
	(void)flag;
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
	(void)name;
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
	std::string modes;
	std::string args = " ";

	if (this->_invite_only)
		modes += 'i';

	if (!this->_key.empty())
	{
		modes += 'k';
		args += this->_key + " ";
	}

	if (this->_limit != 50)
	{
		modes += 'l';
		std::stringstream ss;
		ss << this->_limit;
		args += ss.str();
	}

	if (this->_topic_op_only)
		modes += 't';

	return (modes + args);
}

std::string Channel::getCreat() const
{
	return (this->_creat);
}

std::vector<Client *> &Channel::getMembers()
{
	return (this->_members);
}


//-------------------- Command-related methods-----------------------
int	Channel::addMember(Client &client, std::string const &key) //We need the key arg so that users can enter channels with pasword
{
	(void)key;
	(void)client;
	std::cout << "ADD Channel method WIP" << std::endl;
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
	(void)client;
	(void)target;
	/*
		if client not op
			return ERR_NOTCHANOP
	if target not a member
		 return ERR_USERNOTINCHAN
	call rmMember
		if channel has k flag in _modes
			return ERR_BADCHANKEY
	remove user from all relevant containers
	return SUCCESS
		 */

	std::cout << "KICK Channel method WIP" << std::endl;
	return (SUCCESS);
}

int	Channel::removeMember(Client &client)
{
	std::string nickname = client.getNickname();

	member_iter it = std::find(this->_members.begin(), this->_members.end(), &client);
	if (it == this->_members.end())
		return (ERR_NOTINCHAN);

	this->_members.erase(it);
	this->_operators.erase(nickname);
	this->_invited.erase(nickname);
	return (SUCCESS);
}


int Channel::topicHandle(Client &client, std::vector<std::string> const &msg)
{
	(void)client;
	(void)msg;
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
	bool on_off = (sign == '+'); // renamed "adding" to "on_off" for more clarity

	if (this->isOperator(client.getNickname()))
		return (ERR_NOTCHANOP);
	if (this->needsArg(sign, flag) && arg.empty()) // this prevents us from doing if (!arg.empty()) return (ERR_NEEDMOREPARAMS) in every condition
		return (ERR_NEEDMOREPARAMS);

	switch (flag)
	{
		case 'i':
			this->_invite_only = on_off; break;
		case 't':
			this->_topic_op_only = on_off; break;
		case 'k':
			this->_key.clear(); // used with "-" sign, k removes the need for a key
			if (on_off)
				this->_key = arg;
			break;
		case 'l':
			if (on_off && (arg.find_first_not_of (DIGIT_CHARS) != arg.npos || atoi(arg.c_str()) > 50))
					return (ERR_UNKNOWNMODE);
			this->_limit = 50;
			if (on_off)
				this->_limit = atoi(arg.c_str());
			break;
		case 'o':
			if (!this->findMember(arg))
				return (ERR_USERNOTINCHAN);
			if (on_off)
				this->_operators.insert(arg); // no need to check if already in, sets don't allow duplicates
			else
				this->_operators.erase(arg);
			break;
		default:
			return (ERR_UNKNOWNMODE);
	}

	return (SUCCESS);
}

void Channel::updateNickname(std::string oldnick, std::string newnick)
{
	(void)oldnick;
	(void)newnick;
	/* triggered when a user changes nickname
		remove oldname from channel containers
		add newname to channel containers
		no need to do anything about Client * -> the pointer is still valid */
}
void Channel::removeOperator(std::string target)
{
	if (this->_operators.find(target) == this->_operators.end())
		return ;

	this->_operators.erase(target);// if they weren't op in the first place, we're supposed to ignore and not send error
}
//------------------------- Utils----------------------------
bool	Channel::isEmpty() const
{
	return (this->_members.empty());
}

bool	Channel::isMember(Client &client) const
{
	(void)client;
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
	(void)flag;
	(void)sign;
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


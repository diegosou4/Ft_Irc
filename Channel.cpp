/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 20:54:27 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/04 19:21:58 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

//----------------- Constructor/Destructor ------------------
Channel::Channel(std::string const &name): _name(name), _limit(50), _invite_only(false), _topic_op_only(false)
{
	this->setCreat();
}

Channel::Channel(std::string const &name, int const limit): _name(name), _limit(limit), _invite_only(false), _topic_op_only(false)
{
	this->setCreat();
}

Channel::~Channel()
{
	this->_members.clear();
	this->_invited.clear();
	this->_operators.clear();
}

//------------------------ Setters --------------------------

void Channel::setCreat()
{
	time(&this->_creat); // sets current time
}

void	Channel::setName(std::string const &name)
{
	this->_name = name;
}

void	Channel::setTopic(std::string const &topic)
{
	this->_topic = topic;
}

void	Channel::setKey(std::string const &key)
{
	this->_key = key;
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
}

int Channel::setInvited(std::string const &name)
{
	if (this->findMember(name))
		return (ERR_USERINCHAN);

	this->_invited.insert(name);

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
	std::string args;

	if (this->_invite_only)
		modes += 'i';

	if (!this->_key.empty())
	{
		modes += 'k';
		args += " " + this->_key;
	}

	if (this->_limit != 50)
	{
		modes += 'l';
		std::stringstream ss;
		ss << this->_limit;
		args += " " + ss.str();
	}

	if (this->_topic_op_only)
		modes += 't';

	if (!modes.empty())
		modes = "+" + modes + args;

	return (modes);
}

std::string Channel::getCreat() const
{
	std::ostringstream oss;
	oss << this->_creat;
	return oss.str();
}

std::vector<Client *> &Channel::getMembers()
{
	return (this->_members);
}


//-------------------- Command-related methods-----------------------
int	Channel::addMember(Client &client, std::string const &key)
{
	if (this->isMember(client))
		return (ERR_USERINCHAN);

	if (this->_invite_only && this->_invited.find(client.getNickname()) == this->_invited.end())
		return (ERR_INVITEONLYCHAN);

	if (this->_members.size() == this->_limit)
		return (ERR_CHANISFULL);

	if (!this->_key.empty() && (key.empty() || key != this->_key))
		return (ERR_BADCHANKEY);

	this->_members.push_back(&client);
	this->_invited.erase(client.getNickname());

	if (this->_members.size() == 1)
		this->_operators.insert(client.getNickname());

	return (SUCCESS);
}

int Channel::kickMember(Client &client, std::string const &target)
{
	if (!this->isOperator(client.getNickname()))
		return (ERR_NOTCHANOP);

	if (!this->findMember(target))
		return (ERR_USERNOTINCHAN);

	//this->removeMember(*this->findMember(target));

	return (SUCCESS);
}

int	Channel::removeMember(Client &client)
{
	std::string nickname = client.getNickname();

	member_iter it = std::find(this->_members.begin(), this->_members.end(), &client);
	if (it == this->_members.end())
		return (ERR_NOTINCHAN);

	this->_members.erase(it);
	this->_invited.erase(nickname);
	this->removeOperator(nickname);

	return (SUCCESS);
}

int Channel::topicHandle(Client &client, std::string arg)
{
	if (arg.empty())
	{
		if (this->_topic.empty())
			return (RPL_NOTOPIC);
		return (RPL_TOPIC);
	}

	if (!this->isOperator(client.getNickname()))
		return (ERR_NOTCHANOP);

	this->setTopic(arg);

	return (SUCCESS);
}

int	Channel::modeFlags(Client &client, char sign, char flag, std::string arg)
{
	bool on_off = (sign == '+');

	if (!this->isOperator(client.getNickname()))
		return (ERR_NOTCHANOP);
	if (this->needsArg(sign, flag) && arg.empty())
		return (ERR_NEEDMOREPARAMS);

	switch (flag)
	{
		case 'i':
			this->_invite_only = on_off; break;
		case 't':
			this->_topic_op_only = on_off; break;
		case 'k':
			this->_key.clear();
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
				this->_operators.insert(arg);
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
	if (this->_operators.find(oldnick) != this->_invited.end())
		this->_operators.insert(newnick);

	this->_operators.erase(oldnick);

	if (this->_invited.find(oldnick) != this->_invited.end())
		this->_operators.insert(newnick);

	this->_operators.erase(newnick);
}

void Channel::removeOperator(std::string target)
{
	if (!this->_members.empty() && this->_operators.size() == 1 && this->isOperator(target))
		this->_operators.insert((*this->_members.begin())->getNickname());

	this->_operators.erase(target);
}

//------------------------- Utils----------------------------
bool	Channel::isEmpty() const
{
	return (this->_members.empty());
}

bool	Channel::isMember(Client &client) const
{
	if (std::find(this->_members.begin(), this->_members.end(), &client) != this->_members.end())
		return (true);

	return (false);
}

bool	Channel::isOperator(std::string nick) const
{
	if (this->_operators.find(nick) != this->_operators.end())
		return (true);

	return (false);
}

bool	Channel::needsArg(char sign, char flag)
{
	if (sign == '+' && strchr("klo", flag))
		return (true);

	if (sign == '-' && flag == 'o')
		return (true);

	return (false);
}

Client *Channel::findMember(std::string name)
{
	member_iter it = this->_members.begin();

	for (; it != this->_members.end(); ++it)
		if ((*it)->getNickname() == name)
			return (*it);

	return (NULL);
}


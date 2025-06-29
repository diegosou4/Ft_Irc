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
	if (sign == '+')
	{
		if (_modes.find(flag) == std::string::npos)
			_modes += flag;
	}
	else if (sign == '-')
	{
		size_t pos = _modes.find(flag);
		if (pos != std::string::npos)
			_modes.erase(pos, 1);
	}
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
	if (this->findMember(name))
		return ERR_USERINCHAN;
	_invited.insert(name);
	return SUCCESS;
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
int Channel::addMember(Client &client)
{
	for (std::vector<Client *>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		if ((*it)->getNickname() == client.getNickname())
			return ERR_USERINCHAN;
	}
	if (_limit > 0 && _members.size() >= static_cast<size_t>(_limit))
		return ERR_CHANISFULL;

	if (_invite_only)
	{
		std::set<std::string>::iterator invited = _invited.find(client.getNickname());
		if (invited == _invited.end())
			return ERR_INVITEONLYCHAN;
		_invited.erase(invited);
	}
	_members.push_back(&client);

	return SUCCESS;
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

int Channel::removeMember(Client &client)
{
	std::string nick = client.getNickname();

	for (std::vector<Client *>::iterator it = _members.begin(); it != _members.end(); ++it)
	{
		if ((*it)->getNickname() == nick)
		{
			_members.erase(it);
			_operators.erase(nick);
			_invited.erase(nick);

			return SUCCESS;
		}
	}
	return ERR_USERNOTINCHANNEL;
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

int Channel::modeFlags(Client &client, char sign, char flag, std::string arg)
{
	bool adding = (sign == '+');

	// Exige que o client seja operador
	if (!this->isOperator(client.getNickname()))
		return ERR_CHANOPRIVSNEEDED;

	switch (flag)
	{
		case 'i': // Invite-only
			this->_invite_only = adding;
			break;

		case 't': // Tópico só pode ser alterado por OPs
			this->_topic_op_only = adding;
			break;

		case 'k': // Senha
			if (adding)
			{
				if (arg.empty())
					return ERR_NEEDMOREPARAMS;
				this->setPassword(arg);
			}
			else
				this->setPassword("");
			break;

		case 'l': 
			if (adding)
			{
				if (arg.empty() || arg.find_first_not_of("0123456789") != std::string::npos)
					return ERR_NEEDMOREPARAMS;

				int limit = atoi(arg.c_str());
				this->setLimit(limit);
			}
			else
				this->setLimit(0);
			break;

		case 'o': // OP
			if (arg.empty())
				return ERR_NEEDMOREPARAMS;
			if (adding)
			{
				if (!this->setOperator(arg))
					return ERR_USERONCHANNEL; // já é OP
			}
			else
			{
				if (!this->removeOperator(arg))
					return ERR_USERNOTINCHANNEL;
			}
			break;

		default:
			return ERR_UNKNOWNMODE;
	}
	return SUCCESS;
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

bool Channel::needsArg(char sign, char flag)
{
	if ((flag == 'k' || flag == 'l') && sign == '+')
		return true;
	if (flag == 'o' || flag == 'v' || flag == 'b')
		return true;
		
	if (flag == 'i' || flag == 't' || flag == 'n' ||
		flag == 's' || flag == 'p' || flag == 'm')
		return false;

	return false;
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

bool Channel::removeOperator(std::string const &nick)
{
	if(!this->isOperator(nick))
		return (false);
	this->_operators.erase(nick);
	return (true);
}
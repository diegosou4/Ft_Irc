/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 20:54:27 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/28 22:13:08 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

//----------------- Constructor/Destructor ------------------
Channel::Channel(std::string const &name): _name(name), _limit(50)
{}

Channel::Channel(std::string const &name, int const limit): _name(name), _limit(limit), _topic_op_only(false), _invite_only(false)  // default values
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

// bool	Channel::setBanned(std::string target) //Ban is not compulsory in subject
// {
// 	if (this->_banned.find(target) != this->_banned.end())
// 		return (false);

// 	member_iter it = this->_members.begin();
// 	for (; it != this->_members.end(); ++it)
// 		if ((*it)->getNickname() == target)
// 			break;

// 	if (it != this->_members.end())
// 		this->_members.erase(it);

// 	if (this->isOperator(target))
// 		this->_operators.erase(target);

// 	this->_banned.insert(target);

// 	return (true);
// }


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
	std::string modeStr = msg[1];
	bool adding;

	  if (modeStr[0] == '+')
        adding = true;
    else if (modeStr[0] == '-')
        adding = false;
    else
        return ERR_UNKNOWNMODE;

    for (size_t i = 1; i < modeStr.length(); i++)
    {
        char flag = modeStr[i];
        std::string arg = (msg.size() > 2) ? msg[2] : "";

        int ret = this->modeFlags(client, flag, arg, adding);
        if (ret != SUCCESS)
            return ret;

        // Se a flag consome argumento (como 'o' ou 'k'), pular para próximo argumento em msg[]
        // Para um tratamento completo, você precisa controlar o índice do argumento dinamicamente
    }
    return SUCCESS;
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
int Channel::modeFlags(Client &client, char flag, std::string arg, bool adding)
{
    if (!this->isOperator(client.getNickname()))
        return ERR_NOTCHANOP;

    switch(flag)
    {
        case 'i':
            if (!arg.empty())
                return ERR_NEEDMOREPARAMS;
            this->_invite_only = adding;
            return SUCCESS;

        case 't':
            if (!arg.empty())
                return ERR_NEEDMOREPARAMS;
            this->_topic_op_only = adding;
            return SUCCESS;

        case 'k':
            if (adding)
            {
                if (arg.empty())
                    return ERR_NEEDMOREPARAMS;
                this->setPassword(arg);
            }
            else
            {
                if (!arg.empty())
                    return ERR_NEEDMOREPARAMS;
                this->setPassword("");
            }
            return SUCCESS;

        case 'l':
            if (adding)
            {
                if (arg.empty())
                    return ERR_NEEDMOREPARAMS;
                if (arg.find_first_not_of(DIGIT_CHARS) != std::string::npos)
                    return ERR_UNKNOWNMODE;
                int limit = atoi(arg.c_str());
                if (limit <= 0 || limit > 50)
                    return ERR_UNKNOWNMODE;
                this->setLimit(limit);
            }
            else
            {
                if (!arg.empty())
                    return ERR_NEEDMOREPARAMS;
                this->setLimit(0); // 0 = sem limite
            }
            return SUCCESS;

        case 'o':
            if (arg.empty())
                return ERR_NEEDMOREPARAMS;
            if (!this->findMember(arg))
                return ERR_NOSUCHNICK;
            if (adding)
            {
                if (!this->setOperator(arg))
                    return ERR_USERALREADYOP; // Custom erro code            }
            else
            {
                if (!this->removeOperator(arg))
                    return ERR_USERNOTOP; // Custom erro code
            }
            return SUCCESS;
		}
        default:
            return ERR_UNKNOWNMODE;
    
	}
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

std::string Channel::getModes() const
{
	//return (this->_modes);
	return ("channel->_modes to be implemented");
}

std::string Channel::getCreat() const
{
	//return (this->_modes);
	return ("channel->_creat to be implemented");
}

bool Channel::removeOperator(std::string target)
{
	if (this->_operators.find(target) == this->_operators.end())
		return (false);

	this->_operators.erase(target);
	return (true);
}
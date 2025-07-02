/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: feden-pe <feden-pe@student.42lisboa.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 20:32:16 by feden-pe          #+#    #+#             */
/*   Updated: 2025/07/02 14:50:33 by feden-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channel.hpp"

//----------------- Constructor/Destructor ------------------
Channel::Channel(std::string const &name): _name(name), _limit(50), _invite_only(false), _topic_op_only(false) {
	this->_topic = "";
	this->_key = "";
	this->_limit = 50;
	this->_creat = "";
	setCreat();
}


Channel::Channel(std::string const &name, int const limit): _name(name), _limit(limit), _invite_only(false), _topic_op_only(false)  {
	this->_topic = "";
	this->_key = "";
	this->_limit = limit;
	this->_creat = "";
	setCreat();
}

Channel::~Channel() {
	_members.clear();
	_invited.clear();
	_operators.clear();
}

//------------------------ Setters --------------------------
void	Channel::setCreat() {
	char buff[26];

	time_t current_time = time(NULL);
	struct tm *timeinfo = localtime (&current_time);

	strftime(buff, 26, "%Y-%m-%d %H:%M:%S", timeinfo);

	_creat = buff;
}

void	Channel::setName(std::string const &name) {
	_name = name;
}

void	Channel::setTopic(std::string const &topic) {
	_topic = topic;
}

void	Channel::setKey(std::string const &key) {
	_key = key;
}

void	Channel::setLimit(int const &limit) {
	_limit = limit;
}

bool	Channel::setOperator(std::string target) {
	if (isOperator(target))
		return (false);

	_operators.insert(target);
	return (true);
}

int Channel::setInvited(std::string const &name) {
	if (findMember(name))
		return (ERR_USERINCHAN);

	_invited.insert(name);

	return (SUCCESS);
}

//------------------------ Getters---------------------------
std::string	Channel::getName() const
{
	return (_name);
}

std::string	Channel::getTopic() const
{
	return (_topic);
}

std::string Channel::getModes() const
{
	std::string modes;
	std::string args = " ";

	if (_invite_only)
		modes += 'i';
	if (!_key.empty()) {
		modes += 'k';
		args += _key + " ";
	}
	if (_limit != 50) {
		modes += 'l';
		std::stringstream ss;
		ss << _limit;
		args += ss.str();
	}
	if (_topic_op_only)
		modes += 't';
	return (modes + args);
}

std::string Channel::getCreat() const {
	return (_creat);
}

std::vector<Client *> &Channel::getMembers() {
	return (_members);

}
//-------------------- Command-related methods-----------------------

int	Channel::addMember(Client &client, std::string const &key) {
	if (isMember(client))
		return (ERR_USERINCHAN);

	if (_invite_only && _invited.find(client.getNickname()) == _invited.end())
		return (ERR_INVITEONLYCHAN);

	if (_members.size() == _limit)
		return (ERR_CHANISFULL);

	if (!_key.empty() && (key.empty() || key != _key))
		return (ERR_BADCHANKEY);

	_members.push_back(&client);
	_invited.erase(client.getNickname());

	if (_members.size() == 1)
		_operators.insert(client.getNickname());
	std::cout << "Client " << client.getNickname() << " joined channel " << _name << std::endl;
	return (SUCCESS);
}

int Channel::kickMember(Client &client, std::string const &target) {
	std::cout << "KICK Channel method WIP" << std::endl;
	return (SUCCESS);

	if (!isOperator(client.getNickname()))
		return (ERR_NOTCHANOP);

	if (!findMember(target))
		return (ERR_USERNOTINCHAN);

	removeMember(*findMember(target));

	return (SUCCESS);
}

int	Channel::removeMember(Client &client) {
	std::string nickname = client.getNickname();

	member_iter it = std::find(_members.begin(), _members.end(), &client);
	if (it == _members.end())
		return (ERR_NOTINCHAN);

	_members.erase(it);
	_invited.erase(nickname);
	removeOperator(nickname);

	return (SUCCESS);
}

int Channel::topicHandle(Client &client, std::string arg) {
	if (arg.empty()) {
		if (_topic.empty())
			return (RPL_NOTOPIC);
		return (RPL_TOPIC);
	}

	if (!isOperator(client.getNickname()))
		return (ERR_NOTCHANOP);

	setTopic(arg);

	return (SUCCESS);
}

int	Channel::modeFlags(Client &client, char sign, char flag, std::string arg) {
	bool on_off = (sign == '+');

	if (isOperator(client.getNickname()))
		return (ERR_NOTCHANOP);
	if (needsArg(sign, flag) && arg.empty())
		return (ERR_NEEDMOREPARAMS);

	switch (flag) {
		case 'i':
			_invite_only = on_off; break;
		case 't':
			_topic_op_only = on_off; break;
		case 'k':
			_key.clear();
			if (on_off)
				_key = arg;
			break;
		case 'l':
			if (on_off && (arg.find_first_not_of (DIGIT_CHARS) != arg.npos || atoi(arg.c_str()) > 50))
					return (ERR_UNKNOWNMODE);
			_limit = 50;
			if (on_off)
				_limit = atoi(arg.c_str());
			break;
		case 'o':
			if (!findMember(arg))
				return (ERR_USERNOTINCHAN);
			if (on_off)
				_operators.insert(arg);
			else
				_operators.erase(arg);
			break;
		default:
			return (ERR_UNKNOWNMODE);
	}
	return (SUCCESS);
}

void Channel::updateNickname(std::string oldnick, std::string newnick) {
	if (_operators.find(oldnick) != _invited.end())
		_operators.insert(newnick);

	_operators.erase(oldnick);

	if (_invited.find(oldnick) != _invited.end())
		_operators.insert(newnick);

	_operators.erase(newnick);
}
void Channel::removeOperator(std::string target) {
	if (!_members.empty() && _operators.size() == 1 && isOperator(target))
		_operators.insert((*_members.begin()+1)->getNickname()); // set oldest member as op

	_operators.erase(target); // if they weren't op in the first place, we're supposed to ignore and not send error
	//no need to add a if (this->_operators.find(target) == this->_operators.end()) condition, .erase() handles it for us
}
//------------------------- Utils----------------------------
bool	Channel::isEmpty() const {
	return (_members.empty());
}

bool	Channel::isMember(Client &client) const {
	if (std::find(_members.begin(), _members.end(), &client) != _members.end())
		return (true);

	return (false);
}

bool	Channel::isOperator(std::string nick) const {
	if (_operators.find(nick) != _operators.end())
		return (true);

	return (false);
}

bool	Channel::needsArg(char sign, char flag) {
	if (sign == '+' && strchr("klo", flag))
		return (true);

	if (sign == '-' && flag == 'o')
		return (true);

	return (false);
}

Client *Channel::findMember(std::string name) {
	member_iter it = _members.begin();

	for (; it != _members.end(); ++it)
		if ((*it)->getNickname() == name)
			return (*it);

	return (NULL);
}





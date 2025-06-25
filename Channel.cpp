#include "Channel.hpp"

Channel::Channel(const std::string &name) : _name(name), _limit(50) {
}

Channel::Channel(const std::string &name, const int limit) : _name(name), _limit(limit) {
}

Channel::~Channel() { }

std::vector<Client *>	Channel::getMembers(void) const {
	return (_members);
}

void	Channel::setName(const std::string &name) {
	this->_name = name;
}

std::string	Channel::getName(void) const {
	return (this->_name);
}

std::string	Channel::setTopic(Client &op, const std::vector<std::string> &msg) {
	if (!isOperator(op.getNickname()))
		return ("");
	this->_topic = &msg[2][1];
	return (PURPLE + op.getUsername() + R " changed " + this->_name + " topic to " + &msg[2][1]);
}

std::string	Channel::getTopic(void) const {
	return (this->_topic);
}

void		Channel::setPassword(const std::string &password) {
	this->_password = password;
}

void		Channel::setLimit(const int &limit) {
	this->_limit = limit;
}

std::string		Channel::addClient(Client &client, std::vector<std::string> msg) {
	(void) msg;
	// int	fd = client->getFd();
	std::string nick = client.getNickname();

	if (_banned.find(nick) != _banned.end()) {
		// send message to client saying that they're banned from this channel
		return ("");
	}
	if (_members.size() > _limit) {
		// send message to client saying that the channel is full
		return ("");
	}
	if (std::find(_members.begin(), _members.end(), &client) != _members.end()) {
		return (RED + client.getUsername() + R " is already in channel " + this->_name);
	}
	if (_inviteOnly && _invited.find(nick) == _invited.end()) {
		// send message to client saying that the channel is invite only (+i)
		return ("");
	}
	// if (!_password.empty() && password != _password) {
	// 	// send message to client saying that password is incorrect (+k)
	// 	return ;
	// }
	_members.push_back(&client);
	if (_members.size() == 1)
		_operators.insert(nick);
	//broadcast JOIN to channel
	return (PURPLE + client.getUsername() + R " joined " + _name);
	// if (!_topic.empty())
		// broadcast topic message to new client
}

// void		Channel::broadcast(const std::string &message, Client *exclude) {
// 	for (std::map<int, Client *>::iterator it = _members.begin(); it != _members.end(); ++it) {
// 		if (exclude && it->second == exclude)
// 			continue;
// 		// it->second->sendMessage(message);
// 	}
// }

// PART
void		Channel::removeClient(Client *client) {
	// int fd = client->getFd();
	std::string nick = client->getNickname();

	for (std::vector<Client *>::iterator it = _members.begin(); it != _members.end(); ++it) {
		if ((*it)->getNickname() == nick) {
			_members.erase(it);
			break;
		}
	}
	_operators.erase(nick);
	// _invited.erase(fd); unsure if needed or not
	
	std::string part = ":" + client->getNickname() + " PART " + _name;
	// broadcast(part, client);

	if (_members.empty()) {
		//either destroy channel or have Server do something with it
		return ;
	}
	
	if (_operators.empty() && !_members.empty()) {
		_operators.insert(_members.front()->getNickname());
		// Warn user that they became operators
		// _members.begin()->second->sendMessage();
	}

}

void	Channel::setBanned(Client *op, Client *target) {	
	// Check to see if op is an operator
	if (!isOperator(op->getNickname()))
		return ;
	if (_banned.find(target->getNickname()) == _banned.end())
		_banned.insert(target->getNickname());
}

void	Channel::unban(Client *op, Client *target) {
	// Check to see if op is an operator
	if (!isOperator(op->getNickname()))
		return ;
	if (_banned.find(target->getNickname()) != _banned.end())
		_banned.erase(target->getNickname());
}

void	Channel::setOperator(Client *op, Client *client) {
	// Check to see if is op is an operator
	if (!isOperator(op->getNickname()))
		return ;
	_operators.insert(_operators.end(), client->getNickname());
}

bool	Channel::isOperator(std::string nick) const {
	return (_operators.find(nick) != _operators.end());
}

bool	Channel::isEmpty(void) const {
	return (_members.empty());
}

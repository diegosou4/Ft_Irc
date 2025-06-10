#include "Channel.hpp"

Channel::Channel(const std::string &name) : _name(name) {

}

Channel::~Channel() { }

void	Channel::setName(const std::string &name) {
	this->_name = name;
}

std::string	Channel::getName(void) const { return (this->_name); }

void	Channel::setTopic(const std::string &topic) {
	this->_topic = topic;
}

std::string	Channel::getTopic(void) const { return (this->_topic); }

void		Channel::setPassword(const std::string &password) {
	this->_password = password;
}

void		Channel::addClient(Client *client, const std::string &password) {
	int	fd = client->getFd();

	if (_banned.find(fd) != _banned.end()) {
		// send message to client saying that they're banned from this channel
		return ;
	}
	if (_clients.find(fd) != _clients.end()) {
		// send message to client saying that they're already in the channel
		return ;
	}
	if (_inviteOnly && _invited.find(fd) == _invited.end()) {
		// send message to client saying that the channel is invite only (+i)
		return ;
	}
	if (!_password.empty() && key != _password) {
		// send message to client saying that password is incorrect (+k)
		return ;
	}
	_clients[fd] = client;
	if (_clients.size() == 1)
		_operators.insert(fd);

	//broadcast JOIN to channel
	std::string join = ":" + client->getPrefix() + " JOIN: " + _name;
	broadcast(join, NULL);

	if (!_topic.empty())
		// broadcast topic message to new client
}

void		Channel::broadcast(const std::string &message, Client *exclude) {
	for (std::map<int, Client *>::iterator it = _clients.begin(); it != _clients.end(); ++it) {
		if (exclude && it->second == exclude)
			continue;
		// it->second->sendMessage(message);
	}
}

void		Channel::removeClient(Client *client) {
	int fd = client->getFd();

	if (_clients.find(fd) == _clients.end())
		return ;
	_clients.erase(fd);
	_operators.erase(fd);
	// _invited.erase(fd); unsure if needed or not
	
	std::string part = ":" + client->getPrefix() + " PART " + _name;
	broadcast(part, client);

	if (_clients.empty()) {
		//either destroy channel or have Server do something with it
		return ;
	}
	
	if (_operators.empty() && !_clients.empty()) {
		_operators.insert(_clients.begin()->first);
		// Warn user that they became operators
		_clients.begin()->second->sendMessage();
	}

}

void	Channel::setBanned(Client *op, Client *target) {	
	// Check to see if op is an operator
	if (!isOperator(op->getFd()))
		return ;
	if (_banned.find(target->Fd()) == _banned.end())
		_banned.insert(target->Fd());
}

void	Channel::unban(Client *op, Client *target) {
	// Check to see if op is an operator
	if (!isOperator(op->getFd()))
		return ;
	if (_banned.find(target->Fd()) != _banned.end())
		_banned.erase(target->Fd());
}

void	Channel::setOperator(Client *op, Client *client) {
	// Check to see if is op
	if (!isOperator(op->getFd()))
		return ;
	_operators.insert(_operators.end(), op->getFd());
}

bool	Channel::isOperator(int fd) const { return (_operators.find(fd) != _operators.end()); }

bool	Channel::isEmpty(void) const { return (_clients.empty()); }

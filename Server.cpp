/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/03 21:29:56 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/29 23:34:13 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

//----------------------Constructors/Destructors-------------------------------

// Constructs + sets up all server attributes
Server::Server(int port, std::string password): _port(port), _name("ircserv"), _password(password)
{
	this->setCmdMaps();
	this->_fd = socket(AF_INET, SOCK_STREAM, 0);

	if (this->_fd < 0)
		throw (std::runtime_error("Server socket creation failed"));

	this->setSocket(htons(port), inet_addr(LOCALHOST));

	this->printServer(NULL, GREY "Server has been properly set up");
}

// Private copy constructor to force compilation error
Server::Server(Server const &src)
{
	throw (std::runtime_error("Copy constructor not allowed!"));
	(void)src;
}

// Destroys + frees memory and clears containers
Server::~Server()
{
	if (this->_fd > 0)
		close(this->_fd);
	this->_fds.clear();

	for (clients_iter it = this->_clients.begin(); it != this->_clients.end(); ++it)
		if (it->second)
			delete it->second;
	this->_clients.clear();

	for (channels_iter it = this->_channels.begin(); it != this->_channels.end(); ++it)
		if (it->second)
			delete it->second;
	this->_channels.clear();
}

//-------------------------Operator overloads----------------------------------

// Private assignment operator to force compilation error
Server	&Server::operator=(Server const &src)
{
	throw (std::runtime_error("Assignment operator not allowed!"));

	(void)src;
	return (*this);
}

//---------------------------Getters/Setters-----------------------------------

// Returns server socket fd
int Server::getFd()const
{
	return (this->_fd);
}

// Returns port on which server is listening
int	Server::getPort()const
{
	return (this->_port);
}

// Returns client from _clients container that matches given fd
Client *Server::getClient(int fd)
{
	for (clients_iter it = this->_clients.begin(); it != this->_clients.end(); ++it)
	if (it->second->getFd() == fd)
		return (it->second);

	this->printServer(NULL, RED "No client found with requested fd");
	return (NULL);
}

//-----------------------------Init/setup--------------------------------------

// Sets socket options + defines socket address struct
void	Server::setSocket(in_port_t port, in_addr_t ip)
{
	int opt = 1;
	setsockopt(this->_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	this->_addr.sin_family = AF_INET;
	this->_addr.sin_addr.s_addr = ip;
	this->_addr.sin_port = port;

	this->_gen_addr = (struct sockaddr *)&this->_addr;
	this->_addrlen = sizeof(this->_addr);
}

// Launches server, listens to pollfd activity, adds new clients and treats events
void	Server::handleClient(size_t max_fds, int timeout)
{
	this->initServer(max_fds);

	while (true)
	{
		if (poll(this->_fds.data(), this->_fds.size(), timeout) < 0)
			throw (std::runtime_error("Poll failed"));

		if (this->_fds[0].revents & POLLIN)
		{
			if (this->_fds.size() >= max_fds -1) // I didn't find function
				throw (std::runtime_error("All client slots are taken!"));

			this->addSocket(true);
		}

		this->treatRevent();
	}
}

// Binds socket fd with local address, prepares to listen to connections, add server socket to pollfd vector
void	Server::initServer(int max_fds)
{
	if (bind(this->_fd, this->_gen_addr, this->_addrlen) < 0)
		throw (std::runtime_error("Server binding failed"));

	if (listen(this->_fd, max_fds) < 0)
		throw(std::runtime_error("Server fails to listen"));

	this->addSocket(false);
}

// Adds new socket to pollfd vector + adds it to _client map & sets to non-blocking if is a client
void	Server::addSocket(bool isclient)
{
	struct pollfd newpoll = {};
	newpoll.events = POLLIN;

	if (!isclient)
		newpoll.fd = this->_fd;
	else
	{
		newpoll.fd = accept(this->_fd, this->_gen_addr, &this->_addrlen);
		if (newpoll.fd < 0)
			throw (std::runtime_error("Server failed to accept client connection"));

		fcntl(newpoll.fd, F_SETFL, O_NONBLOCK);

		Client *newclient = new Client(newpoll.fd);
		newclient->setState(AT_DOOR);
		this->_clients[newclient->getNickname()]= newclient; //use fd (as string) as key OR UICIDSS

		this->printServer(newclient, "is at the door");
		this->sendNumeric(*newclient, RPL_WELCOME, WELCOME);
	}

	this->_fds.push_back(newpoll);
}


void	Server::setCmdMaps()
{
	this->_authcmds["PING"] = &Server::cmdPing;
	this->_authcmds["PASS"] = &Server::cmdPass;
	this->_authcmds["NICK"] = &Server::cmdNick;
	this->_authcmds["USER"] = &Server::cmdUser;
	this->_authcmds["JOIN"] = &Server::cmdJoin;
	this->_authcmds["INVITE"] = &Server::cmdInvite;
	this->_authcmds["KICK"] = &Server::cmdKick;
	this->_authcmds["PART"] = &Server::cmdPart;
	this->_authcmds["QUIT"] = &Server::cmdQuit;
	this->_authcmds["NAME"] = &Server::cmdNames;
	this->_authcmds["PRIVMSG"] = &Server::cmdPrivmsg;
	this->_authcmds["TOPIC"] = &Server::cmdTopic;
	this->_authcmds["MODE"] = &Server::cmdMode;
}

// Retrieves message from user + perform checks - removes client if errors
std::string Server::getMsg(Client &client)
{
	char buff[BUFFSIZE];
	memset(buff, 0, BUFFSIZE);

	int bytes_read = recv(client.getFd(), buff, BUFFSIZE, 0);

	if (bytes_read <= 0)
	{
		if (bytes_read < 0)
			this->printServer(&client, RED + std::string(strerror(errno)));
		this->cmdQuit(&client, NULL, newVector("QUIT", ":Dropped unexpectedly", 0));
		buff[0] = 0;
	}

	return (std::string(buff));
}

// Splits user command into vector string items
Server::str_vector Server::splitMsg(std::string &msg)
{
	std::string str;
	str_vector split_msg;
	std::stringstream ss(msg);

	while (ss >> str)
	{
		if (str[0] == ':')
		{
			std::string arg;
			std::getline(ss, arg);
			if (!arg.empty())
				str += arg;
			split_msg.push_back(str);
			break;
		}
		split_msg.push_back(str);
	}

	return (split_msg);
}

// Removes client from pollfd + _clients & deletes it
void	Server::removeClient(Client *client)
{
	for (pollfd_iter it = this->_fds.begin(); it != this->_fds.end(); ++it)
		if (client->getFd() == it->fd)
			it->fd = REMOVAL;

	this->printServer(client, "has left");

	this->_clients.erase(client->getNickname());
	close(client->getFd());
	delete client;
}

// Checks if client has passed PASS, NICK and USER to be considered ACTIVE
bool	Server::authCheck(Client &client)
{
	if (client.getState() == ACTIVE)
		return (true);

	if (client.getState() != PASS_OK || !client.passedNick() || !client.passedUser())
		return (false);

	client.setState(ACTIVE);

	this->sendNumeric(client, RPL_WELCOME, WELCOME);
	this->printServer(&client, "has successfully logged in");

	return (true);
}

// Creates vector from 2 or 3 strings
Server::str_vector Server::newVector(std::string const &arg1, std::string const &arg2, std::string *arg3)
{
	str_vector new_vector;

	new_vector.push_back(arg1);
	new_vector.push_back(arg2);
	if (arg3)
		new_vector.push_back(*arg3);

	return (new_vector);
}

// Joins vector indexes into a space separated string
std::string	Server::argExists(str_vector const &msg, size_t index)
{
	if (index >= msg.size())
		return (NULL);

	return (msg[index]);
}





// Performs checks, updates username & realname
void Server::cmdUser(Client *client, Channel *channel, str_vector const &msg)
{
	(void)channel;
	int code = 0;

	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->getState() == ACTIVE || client->passedUser())
		code = ERR_ALREADYAUTHED;
	else if (msg.size() < 5)
		code = ERR_NEEDMOREPARAMS;
	else if (msg.size() > 5 && (msg[4][0] != ':' || msg[4].length() <= 1))
		code = ERR_UNKNOWNCOMMAND;

	if (code)
		return (this->sendNumeric(*client, code));

	if(msg.size() < 4)
		return (this->sendNumeric(*client, ERR_NEEDMOREPARAMS));
	if(msg[1].length() < 1 || msg[2].length() < 1 || msg[3].length() < 1)
		return (this->sendNumeric(*client, ERR_NEEDMOREPARAMS));
	
	client->setUsername(msg[1]);
	client->setHostname(msg[2]);
	client->setPrefix(); 
	client->setState(ACTIVE); 
	client->setRealname(msg[3]); 

	this->authCheck(*client);
}

// Performs checks, sends to PART for all channels that client was in, sends to removal
void Server::cmdQuit(Client *client, Channel *channel, str_vector const &msg)
{
	if (!client)
		throw std::runtime_error("Fatal: client not found");

	if (msg.size() > 1 && (msg[1][0] != ':' || msg[1].length() < 2))
		return (this->sendNumeric(*client, ERR_NEEDMOREPARAMS));

	channels_iter it = this->_channels.begin();
	while (it != this->_channels.end())
	{
		channel = it->second;
		it++;
		if (channel->isMember(*client))
		{
			// std::string goodbye_msg = this->unSplit(msg, 1);
			// this->cmdPart(client, channel, this->newVector(msg[0], channel->getName(), &goodbye_msg));
		}
	}

	this->removeClient(client);
}

// Lists all channel members, sends back string
std::string Server::nameList(Channel &channel)
{
	std::string namelist = "= " + channel.getName() + " :";
	Channel::member_iter it = channel.getMembers().begin();
	for (; it != channel.getMembers().end(); ++it)
	{
		if (channel.isOperator((*it)->getNickname()))
			namelist += " @" + (*it)->getNickname();
		else
			namelist += " " + (*it)->getNickname();
	}

	return (namelist);
}

// Performs basic checks on components
int	Server::cmdCheck(Client *client, Channel *channel, std::string target)
{
	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->getState() != ACTIVE)
		return (ERR_NOTAUTHED);
	else if (!target.empty() && this->_clients.find(target) == this->_clients.end())
		return (ERR_NOSUCHNICK);
	else if (!channel)
		return(ERR_NOSUCHCHAN);
	else if (!channel->isMember(*client))
		return (ERR_NOTINCHAN);

	return (SUCCESS);
}

// Checks if MODE and flag command was sent in correct formatting
int		Server::checkModeFormat(str_vector const &msg)
{
	std::string flags = "ilkot";
	std::string valid_signs = "+-";

	if (msg.size() > 2 && !strchr(valid_signs.c_str(), msg[2][0]))
		return (-1);

	for (size_t i = 2; i < msg.size(); i++)
	{
		if (i != 2 && !strchr(valid_signs.c_str(), msg[i][0]))
			return (i);
		if (msg[i].find_first_not_of(flags + valid_signs) != msg[i].npos)
			return (-1);
		for (int j = 0; msg[i][j]; j++)
			if (strchr(valid_signs.c_str(), msg[i][j]) && (!msg[i][j +1] || !strchr(flags.c_str(), msg[i][j +1])))
				return (-1);
	}

	return (msg.size());
}

// Retrieves sign, associated flag & associated arg to send to channel Mode method
void	Server::sendMode(Client &client, Channel &channel, size_t stop, str_vector const &msg)
{
	size_t limit = stop;
	char sign = msg[2][0];
	std::string valid_signs = "+-";

	for (size_t i = 2; i < limit; i++)
	{
		for (size_t j = 0; msg[i][j]; j++)
		{
			if (strchr(valid_signs.c_str(), msg[i][j]))
			{
				sign = msg[i][j];
				continue;
			}

			std::string arg;
			if (stop < msg.size() && channel.needsArg(sign, msg[i][j]))
				arg = " " + msg[stop++];
			int code = channel.modeFlags(client, sign, msg[i][j], this->argExists(msg, stop -1));
			if (code)
				this->sendNumeric(client, code);
			else
				this->broadcast(client, channel, msg[0], std::string(1, sign) + msg[i][j] + arg);
		}
	}
}

Channel	*Server::findChannel(str_vector &split_msg)
{
	Channel *channel = NULL;
	std::string channel_name;

	if (split_msg.size() >= 2 && split_msg[1][0] == '#')
		channel_name = split_msg[1];
	else if (split_msg.size() >= 3 && split_msg[2][0] == '#' && split_msg[0] == "INVITE")
		channel_name = split_msg[2];

	if (this->_channels.find(channel_name) != this->_channels.end())
		channel = this->_channels[channel_name];

	return (channel);
}

void	Server::treatRevent()
{
	if (this->_fds.size() <= 1)
		return ;

	for (pollfd_iter it = this->_fds.begin() +1; it != this->_fds.end(); ++it)
	{
		Client *client = this->getClient(it->fd);
		if (!client)
			continue ;
		switch (it->revents)
		{
			case POLLHUP:
				this->pollHup(*client); break;
			case POLLIN:
				this->pollIn(*client); break;
			case POLLERR:
				this->pollErr(*client); break;
			case POLLNVAL:
				this->pollNVal(*client); break;
			default:
				break;
		}
		if (it->fd == REMOVAL)
			this->_fds.erase(it--);
	}
}

// POLLUP = client left
void	Server::pollHup(Client &client)
{
	this->cmdQuit(&client, NULL, newVector("QUIT", ":Dropped unexpectedly", 0));
}

// POLLIN = message sent by client
// Retrieves message, splits it into vector, sends it to command managers
void Server::pollIn(Client &client)
{
	std::string msg = getMsg(client);
	
	if (msg.empty())
		return;
	// Split msg by "\r\n" manually
	std::vector<std::string> split_enter;
	size_t start = 0;
	size_t end = 0;
	
	while ((end = msg.find("\r\n", start)) != std::string::npos) { // Camille Check This
		std::string line = msg.substr(start, end - start);
		if (!line.empty())
			split_enter.push_back(line);
		start = end + 2;
	}
	if (start < msg.size()) {
		std::string line = msg.substr(start);
		if (!line.empty())
			split_enter.push_back(line);
	}

	for (size_t i = 0; i < split_enter.size(); ++i)
	{
		std::string &line = split_enter[i];
		std::vector<std::string> cmdArgs = this->splitMsg(line);

		if (cmdArgs.empty())
			continue;

		Channel *channel = findChannel(cmdArgs); 
		std::string cmdName = cmdArgs[0];
		std::cout << "Command received: " << cmdName << std::endl;
		if (this->_authcmds.find(cmdName) != this->_authcmds.end())
			(this->*_authcmds[cmdName])(&client, channel, cmdArgs);
		else 
			this->sendNumeric(client, ERR_UNKNOWNCOMMAND);
	}
	
}


// POLLERR = error occurred with fd
// Depending on error code: ignores, reopen socket, or removes client
void	Server::pollErr(Client &client)
{
	this->printServer(&client, RED  "Error occurred: ");

	if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
	{
		this->printServer(NULL, RED "Retrying ...");
		return ;
	}
	else if (errno == ETIMEDOUT)
	{
		this->printServer(NULL, RED "Reopening attempt ...");
		close(client.getFd());

		client.setFd(socket(AF_INET, SOCK_STREAM, 0));
		if (client.getFd() >= 0)
		{
			fcntl(client.getFd(),  F_SETFL, O_NONBLOCK);
			this->printServer(NULL, PURPLE "Reconnection successful");
			return ;
		}
	}

	this->printServer(NULL, RED "Unrecoverable - closing socket");
	this->cmdQuit(&client, NULL, newVector("QUIT", ":Dropped unexpectedly", 0));
}

// POLLNVAL = file descriptor issue
// Shows error message + sends to client removal
void	Server::pollNVal(Client &client)
{
	this->printServer(&client, RED "Invalid file descriptor\nUnrecoverable - closing socket");
	this->cmdQuit(&client, NULL, newVector("QUIT", ":Dropped unexpectedly", 0));
}

void Server::cmdPass(Client *client, Channel *channel, str_vector const &msg)
{
	(void)channel;
	int code = 0;

	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->getState() >= PASS_OK)
		code = ERR_ALREADYAUTHED;
	else if (msg.size() != 2)
		code = ERR_NEEDMOREPARAMS;
	else if (msg[1] != this->_password)
		code = ERR_WRONGPASS;

	if (code)
		return (this->sendNumeric(*client, code));

	client->setState(PASS_OK);

	this->authCheck(*client);
}

// Checks client existence, state & command format
void Server::cmdNick(Client *client, Channel *channel, str_vector const &msg)
{
	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	(void)channel;
	int code = 0;

	if (msg.size() != 2)
		code = ERR_NEEDMOREPARAMS;
	else if (this->_clients.find(msg[1]) != this->_clients.end() || \
	client->getNickname() == msg[1])
		code = ERR_NICKINUSE;
	else if (msg[1].find_first_not_of(DIGIT_CHARS ALPHA_CHARS) != msg[1].npos)
		code = ERR_INVALIDNICK;

	if (code)
		return (this->sendNumeric(*client, code));

	if (client->getState() == ACTIVE)
		// this->broadcast(*client, msg[0], msg[1]);

	client->setNickname(msg[1]);

	this->authCheck(*client);
}







// Checks client existence, state, command format & channel name format
void Server::cmdJoin(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (client->getState() != ACTIVE)
		code = ERR_NOTAUTHED;
	else if (msg.size() != 2)
		code = ERR_NEEDMOREPARAMS;
	else if (msg[1][0] != '#' || msg[1].length() < 2)
		code = ERR_UNKNOWNCOMMAND;

	if (!channel)
	{
		channel = new Channel(msg[1]);
		this->_channels[msg[1]] = channel;
	}

	if (!code)
		code = channel->addMember(*client);

	if (code)
		return (this->sendNumeric(*client, code));

	this->cmdTopic(client, channel, this->newVector("TOPIC", channel->getName(), 0));
	this->cmdNames(client, channel, this->newVector("NAMES", channel->getName(), 0));

	this->broadcast(*client, *channel, msg[0], channel->getName());
}

void Server::cmdInvite(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;
	Client* target = NULL;

	if (msg.size() != 3)
		code = ERR_NEEDMOREPARAMS;
	else
		code = cmdCheck(client, channel, msg[1]);

	if (!code)
		code = channel->setInvited(msg[1]);

	if (code)
		return (this->sendNumeric(*client, code));

	target = this->_clients[msg[1]];

	this->sendNumeric(*client, RPL_INVITING, target->getNickname() + " " + channel->getName());
	// this->broadcast(*client, target, msg[0], target.getNickname() + " :" + channel->getName());
}

void Server::cmdKick(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;
	std::string reason;

	if (msg.size() < 3)
		code = ERR_NEEDMOREPARAMS;
	else if (msg.size() > 3 && (msg[3][0] != ':' || msg[3].length() < 2))
		code = ERR_NEEDMOREPARAMS;
	else
		code = cmdCheck(client, channel, msg[2]);

	if (!code)
		code = channel->kickMember(*client, msg[2]);

	if (code)
		return (this->sendNumeric(*client, code));

	if (msg.size() > 3)
		// reason = this->unSplit(msg, 3);

	this->broadcast(*client, *channel, msg[0], msg[1] + reason);
}

void Server::cmdPart(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;
	std::string goodbye_msg;

	if (msg.size() < 2)
		code = ERR_NEEDMOREPARAMS;
	else if (msg.size() > 2 && (msg[2][0] != ':' || msg[2].length() < 2))
		code = ERR_NEEDMOREPARAMS;
	else
		code = cmdCheck(client, channel, "");

	if (!code)
		code = channel->removeMember(*client);

	if (code)
		return (this->sendNumeric(*client, code));

	if (msg.size() > 2)
		// goodbye_msg = this->unSplit(msg, 2);

	this->broadcast(*client, *channel, msg[0], goodbye_msg);
}



void Server::cmdNames(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	if (msg.size() == 1)
	{
		channels_iter it = this->_channels.begin();
		for (; it != this->_channels.end(); ++it)
			this->cmdNames(client, it->second, this->newVector("NAMES", it->first, 0));
		return ;
	}
	else if (msg.size() != 2)
		code = ERR_NEEDMOREPARAMS;
	else
		code = cmdCheck(client, channel, "");

	if (code == ERR_NOTINCHAN)
		code = 0;

	if (code)
		return (this->sendNumeric(*client, code));

	this->sendNumeric(*client, RPL_NAMREPLY, this->nameList(*channel));
	this->sendNumeric(*client, RPL_ENDOFNAMES, ":End of /NAMES list.");
}

void Server::cmdPrivmsg(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	if (msg.size() < 3 || (msg[2][0] != ':' || msg[2].size() < 2))
		code = ERR_NEEDMOREPARAMS;
	else
		code = cmdCheck(client, channel, msg[1]);

	if ((code == ERR_NOSUCHNICK && channel) || (code == ERR_NOSUCHCHAN && msg[1][0] != '#'))
		code = 0;

	if (!code && channel && !channel->isMember(*client))
		code = ERR_NOTINCHAN;

	if (code)
		return (this->sendNumeric(*client, code));

	std::string output = "";

	if (channel)
		this->broadcast(*client, *channel, msg[0], output);
	else
		this->broadcast(*client, *this->_clients[msg[1]], msg[0], output);
}

void Server::cmdTopic(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	if (!code && msg.size() < 5)
		code = ERR_NEEDMOREPARAMS;
	else if (!code && msg.size() > 2 && (msg[2][0] != ':' || msg[2].length() <= 1))
		code = ERR_UNKNOWNCOMMAND;
	else
		code = cmdCheck(client, channel, "");

	if (!code)
		code = channel->topicHandle(*client, msg);

	if (code == RPL_NOTOPIC)
		this->sendNumeric(*client, code, channel->getName() + " :No topic is set");
	else if (code == RPL_TOPIC)
		this->sendNumeric(*client, code, channel->getName() + " :" + channel->getTopic());
	else if (code)
		this->sendNumeric(*client, code);
	else
		this->broadcast(*client, *channel, msg[0], channel->getTopic());
}


void Server::cmdMode(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	code = cmdCheck(client, channel, "");
	int stop = checkModeFormat(msg);

	if (!code && stop == -1)
		code = ERR_UNKNOWNCOMMAND;

	if (code)
		return (this->sendNumeric(*client, code));

	if (msg.size() > 2)
		return (this->sendMode(*client, *channel, stop, msg));

	this->sendNumeric(*client, RPL_CHANMODE, channel->getName() + " " + channel->getModes());
	this->sendNumeric(*client, RPL_CREATTIME, channel->getName() + " " + channel->getCreat());
}

void Server::cmdPing(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;
	(void)channel; 
	if (!client)
		throw (std::runtime_error("Fatal: client not found"));

	if (msg.size() != 2)
		code = ERR_NEEDMOREPARAMS;
	else if (msg[1].find_first_not_of(DIGIT_CHARS ALPHA_CHARS) != msg[1].npos)
		code = ERR_UNKNOWNCOMMAND;

	if (code)
		return (this->sendNumeric(*client, code));

	this->sendNumeric(*client, RPL_PONG, msg[1]);
}
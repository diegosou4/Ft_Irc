/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ChanCmds.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 21:35:58 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/02 22:49:45 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//------------------------------ChanCmds---------------------------------------
#include "Server.hpp"

// Performs checks, adds channel didnt exist, adds user to channel, sends relevant messages
void Server::cmdJoin(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	if (!client)
		throw (std::runtime_error("Fatal: client not found"));
	if (client->getState() != ACTIVE)
		code = ERR_NOTAUTHED;
	else if (msg.size() < 2)
		code = ERR_NEEDMOREPARAMS;
	else if (msg[1][0] != '#' || msg[1].length() < 2)
		code = ERR_UNKNOWNCOMMAND;

	if (!code)
	{
		if (!channel)
		{
			channel = new Channel(msg[1]);
			this->_channels[msg[1]] = channel;
		}
		code = channel->addMember(*client, this->argExists(msg, 2));
	}

	if (code)
	{
		std::cout << "Join error: " << code << std::endl;
		return (this->sendNumeric(*client, channel, code));
	}
		

  this->broadcastJoin(*client, *channel);
	this->cmdTopic(client, channel, this->newVector("TOPIC", channel->getName(), 0));
	this->cmdNames(client, channel, this->newVector("NAMES", channel->getName(), 0));

}

// Performs checks, sends to channel invite method, sends relevant messages
void Server::cmdInvite(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	if (msg.size() != 3)
		code = ERR_NEEDMOREPARAMS;
	else
		code = cmdCheck(client, channel, msg[1]);

	if (!code)
		code = channel->setInvited(msg[1]);

	if (code)
		return (this->sendNumeric(*client, code));

	Client target = *this->_clients[msg[1]];

	this->sendNumeric(*client, RPL_INVITING, target.getNickname() + " " + channel->getName());
	this->broadcast(*client, target, msg[0], target.getNickname() + " :" + channel->getName());
}

// Performs checks, sends to channel kick method, sends relevant message
void Server::cmdKick(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;
	std::string reason = " :"; // mesmo sem motivo o servidor envia um ":"
	// Deve enviar o motivo pelo channel e depois disso remover ele do channel
	// :OperadorNick KICK #canal UsuarioExpulso :Motivo aqui
	if (msg.size() < 3)
		code = ERR_NEEDMOREPARAMS;
	else if (msg.size() > 3 && (msg[3][0] != ':' || msg[3].length() < 2))
		code = ERR_NEEDMOREPARAMS;
	else
		code = cmdCheck(client, channel, msg[2]);
	std::cout << "Kick: " << msg[2] << std::endl;
	std::cout << code << std::endl;
	if (!code)
		code = channel->kickMember(*client, msg[2]);

	if (code)
		return (this->sendNumeric(*client, code));

	if (msg.size() > 3)
		reason = " " + this->argExists(msg, 3);

	this->broadcastAll(*client, *channel, "KICK", msg[2] + reason);
	this->broadcast(*client, *channel, msg[0], msg[1] + reason);
}

// Performs checks, sends to channel remove method, deletes channel if empty, sends relevant message
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

	if (!channel->isEmpty())
		return (this->broadcast(*client, *channel, msg[0], this->argExists(msg, 2)));

	this->_channels.erase(channel->getName());
	delete channel;
}

// Performs checks, sends name list & appropriate codes
void Server::cmdNames(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	if (client->getState() == ACTIVE && msg.size() == 1)
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

	if (channel->isEmpty())
		this->sendClient(*client, "Channel is empty");

	this->sendNumeric(*client, RPL_ENDOFNAMES, ":End of /NAMES list.");
}

// Performs checks, sends message to relevant client/channel
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
	std::cout << "Privmsg: " << msg[2] << std::endl;
	
	if (channel)
	{

		this->broadcast(*client, *channel, msg[0], this->argExists(msg, 2));
	}
		
	else
		this->broadcast(*client, *this->_clients[msg[1]], msg[0], this->argExists(msg, 2));
}

// Performs checks, sends to channel topic method, sends relevant messages & codes
void Server::cmdTopic(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	if (!code && msg.size() < 2)
		code = ERR_NEEDMOREPARAMS;
	else if (!code && msg.size() > 2 && (msg[2][0] != ':' || msg[2].length() <= 1))
		code = ERR_UNKNOWNCOMMAND;
	else
		code = cmdCheck(client, channel, "");
	if (code == ERR_NOTINCHAN)
		code = 0;

	if (!code)
		code = channel->topicHandle(*client, this->argExists(msg, 2));
	

	// Changed By Diego
	if( code == ERR_NOTCHANOP) // Working With client
	{
		std::ostringstream oss;
		oss << ":" << this->_name << " "
			<< std::setw(3) << std::setfill('0') << code << " "
			<< client->getNickname() << " "
			<< channel->getName() << " "
			<< ":You're not channel operator\r\nY";		
		this->SendErrorMsg(*client, oss.str());
		return;
	}
	else if (code == RPL_NOTOPIC)
	{
		this->sendNumeric(*client, code, channel->getName() + " :No topic is set");
	}
	else if (code == RPL_TOPIC && msg.size() > 2) // cliente alterou o tópico
	{
		this->sendNumeric(*client, code, channel->getName() + " :" + channel->getTopic());
		this->broadcastAll(*client, *channel, "TOPIC", ":" + channel->getTopic()); // envia pra todos
	}
	else if (code == RPL_TOPIC && msg.size() == 2) // cliente apenas consultou o tópico
	{
		this->sendNumeric(*client, code, channel->getName() + " :" + channel->getTopic());
	}
	else if (code)
	{
		this->broadcastAll(*client, *channel, "TOPIC ", " :" + channel->getTopic());
		this->sendNumeric(*client, code);
		this->broadcast(*client, *channel, msg[0], channel->getTopic());
	}
	else
	{
		this->broadcast(*client, *channel, msg[0], channel->getTopic());
	}

}

// Performs checks, sends to flag dispatch function or sends back relevant messages & codes
void Server::cmdMode(Client *client, Channel *channel, str_vector const &msg)
{
	int code = 0;

	code = cmdCheck(client, channel, client->getNickname());
	std::cout << "Code aqui " << code << std::endl;
	int stop = checkModeFormat(msg);

	if (!code && stop == -1)
		code = ERR_UNKNOWNCOMMAND;
	std::cout << "Mode: " << msg[0] << std::endl;
	std::cout << " Porque o codeg aqui " << code << std::endl;
	if (code)
		return (this->sendNumeric(*client, code));

	// if (msg.size() > 2 && !code && stop == -1) 
	// {
	// 	return (this->sendMode(*client, *channel, stop, msg));
	// }
	if(msg.size() > 2 && code  == 0 && stop != -1)
	{
		if(msg[2][0] == '+' && msg[2][1] == 'o')
			this->setOperatorChannel(*channel, *client);
		else if(msg[2][0] == '-' && msg[2][1] == 'o')
			this->removeOperatorChannel(*channel, *client);
	}
	this->sendNumeric(*client, RPL_CHANMODE, channel->getName() + " " + channel->getModes());
	this->sendNumeric(*client, RPL_CREATTIME, channel->getName() + " " + channel->getCreat());
}

void Server::setOperatorChannel(Channel &channel, Client &client)
{
	std::cout << channel.getName() << " " << channel.hasOperator() << std::endl;
	if(channel.hasOperator() == false)
	{
		channel.setHasOperator(true);
		channel.setOperator(client.getNickname());
		this->broadcastAll(client, channel, "MODE", "+o " + client.getNickname());
	}
	else if(channel.hasOperator() == true && !channel.isOperator(client.getNickname()))
	{
		this->sendNumeric(client, ERR_NOTCHANOP);
	}
}
// With client the fuction working ok but, when use nc segfault i don't know why
void Server::removeOperatorChannel(Channel &channel, Client &client)
{
	if (channel.isOperator(client.getNickname()))
	{
		channel.removeOperator(client.getNickname());
		channel.setHasOperator(false);
		
		this->broadcastAll(client, channel, "MODE", "-o " + client.getNickname());
	}
	else
	{
		this->sendNumeric(client, ERR_NOTCHANOP);
	}
}

void parseOperator(Channel *channel, Client *client, std::string &arg)
{
	if (arg[0] == '+')
	{
		if (channel->setOperator(client->getNickname()))
			channel->setHasOperator(true);
		else
			throw std::runtime_error("Failed to set operator");
	}
	else if (arg[0] == '-')
	{
		channel->removeOperator(arg.substr(1));
	}
	else
		throw std::runtime_error("Invalid operator format");
}
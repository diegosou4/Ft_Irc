/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CmdUtils.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/29 22:00:13 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/06 10:15:04 by feden-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//--------------------------------CmdUtils----------------------------------------
#include "../includes/Server.hpp"

// Searches channel name amongst _channels map, returns NULL if not found
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
				arg = msg[stop++];
			int code = channel.modeFlags(client, sign, msg[i][j], arg);
			if (code == ERR_NOTCHANOP)
				this->sendNumeric(client, &channel, code);
			else if (code)
				this->sendNumeric(client, code, " " + arg + " :" + ErrMsg.find(code)->second);
			else
				this->broadcastAll(client, channel, msg[0], channel.getName() + " " + std::string(1, sign) + msg[i][j] + " " + arg);
		}
	}
}

void Server::leaveAllChans(std::string cmd, Client &client, std::string msg)
{
	channels_iter it = this->_channels.begin();
	while (it != this->_channels.end())
	{
		Channel *channel = it->second;
		it++;
		if (channel->isMember(client))
		{
			if (cmd != "QUIT")
				this->cmdPart(&client, channel, this->newVector("PART", channel->getName(), &msg));
			else
			{
				this->broadcastOthers(client, *channel, cmd, msg);
				channel->removeMember(client);
				if (channel->isEmpty())
				{
					this->_channels.erase(channel->getName());
					delete channel;
				}
			}
		}
	}
}

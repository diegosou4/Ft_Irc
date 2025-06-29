/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 20:54:22 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/28 22:12:49 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <set>
#include <vector>
#include <iostream>
#include <algorithm>
#include "Client.hpp"
#include "Macros.hpp"

class Channel
{
	private:
		std::string	_name;
		std::string _topic;
		std::string _password;
		std::string _modes;//add modes
		std::string _creat;//add creation time
		
		bool	_topic_op_only;

		bool	_invite_only;
		size_t	_limit;

		std::vector<Client *> 	_members;
		std::set<std::string>	_operators;
		std::set<std::string>	_invited;
		std::set<std::string>	_banned; //Ban is not compulsory in subject
		// If we want to follow through with ban, we'll have to do it via hostmask
		// Should we keep at it or remove this functionality?
	public:
		typedef std::vector<Client *>::iterator member_iter;

		//Constructor/Destructor:
		Channel(std::string const &name);
		Channel(std::string const &name, int const limit);
		~Channel();

		//Setters:
		void	setName(std::string const &name);
		void	setTopic(std::string const &topic);
		void	setPassword(std::string const &password);
		void	setLimit(int const &limit);
		bool	setOperator(std::string target);
		int setInvited(std::string &name);

		//Getters:
		std::string	getName() const;
		std::string	getTopic() const;
		std::string getModes() const;
		std::string getCreat() const;
		std::vector<Client *> getMembers() const;

		//Command functions:
		int joinCmd(Client &client);
		int modeCmd(Client &client, std::vector<std::string> &msg);
		int topicHandle(Client &client, std::vector<std::string> &msg);
		int inviteCmd(Client &client, std::vector<std::string> &msg);
		int privmsgCmd(Client &client, std::vector<std::string> &msg);
		int kickCmd(Client &client, std::vector<std::string> &msg);
		//Utils:
		bool	removeOperator(std::string target);
		bool	isEmpty() const;
		bool	isMember(Client &client) const;
		bool	isOperator(std::string nick) const;
		void	unban(Client *op, Client *target);
		Client *findMember(std::string name);
		int	addClient(Client &client);
		int	modeFlags(Client &client, char flag, std::string targetname,bool adding);
);
		int kickMember(Client &client, std::string &target);
};

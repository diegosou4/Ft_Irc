/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 20:54:22 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/27 11:23:38 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <set>
#include <vector>
#include <iostream>
#include <algorithm>
#include "Client.hpp"

#define DIGIT_CHARS	"01234567789"

class Channel
{
	private:
		std::string	_name;
		std::string _topic;
		std::string _password;

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
		bool	setBanned(std::string target);

		//Getters:
		std::string	getName() const;
		std::string	getTopic() const;
		std::vector<Client *> getMembers() const;

		//Command functions:
		std::string joinCmd(Client &client);
		std::string modeCmd(Client &client, std::vector<std::string> &msg);
		std::string topicCmd(Client &client, std::vector<std::string> &msg);
		std::string inviteCmd(Client &client, std::vector<std::string> &msg);
		std::string privmsgCmd(Client &client, std::vector<std::string> &msg);
		std::string kickCmd(Client &client, std::vector<std::string> &msg);

		//Utils:
		bool	isEmpty() const;
		bool	isMember(Client &client) const;
		bool	isOperator(std::string nick) const;
		void	unban(Client *op, Client *target);
		Client *findMember(std::string name);
		std::string	addClient(Client &client);
		std::string	modeFlags(Client &client, char flag, std::string targetname);
};

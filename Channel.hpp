/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 20:54:22 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/02 22:07:48 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// -LIBRARIES-
#include <set>
#include <ctime>
#include <vector>
#include <cstring>
#include <sstream>
#include <iostream>
#include <algorithm>

#include "Client.hpp"
#include "Macros.hpp"

// -CLASS-
class Channel
{
	private:
		std::string	_name;
		std::string _creat;
		std::string _topic;

		size_t _limit;
		std::string _key;
		bool _invite_only;
		bool _topic_op_only;

		std::vector<Client *> 	_members;
		std::set<std::string>	_operators;
		std::set<std::string>	_invited;

	public:
		typedef std::vector<Client *>::iterator member_iter;

		//Constructor/Destructor:
		Channel(std::string const &name);
		Channel(std::string const &name, int const limit);
		~Channel();

		//Setters:
		void	setCreat();
		void	setName(std::string const &name);
		void	setTopic(std::string const &topic);
		void	setKey(std::string const &password);
		void	setLimit(int const &limit);
		void	setModes(char sign, char flag);
		bool	setOperator(std::string target);
		int 	setInvited(std::string const &name);

		//Getters:
		std::string	getName() const;
		std::string	getTopic() const;
		std::string getModes() const;
		std::string getCreat() const;
		std::vector<Client *> &getMembers();

		//Command-related methods:
		int	addMember(Client &client, std::string const &key);
		int	removeMember(Client &client);
		int kickMember(Client &client, std::string const &target);
		int topicHandle(Client &client, std::string arg);
		int	modeFlags(Client &client, char sign, char flag, std::string arg);
		void updateNickname(std::string oldnick, std::string newnick);
		void removeOperator(std::string target);

		//Utils:
		bool	isEmpty() const;
		bool	isMember(Client &client) const;
		bool	isOperator(std::string nick) const;
		bool	needsArg(char sign, char flag);
		Client *findMember(std::string name);
};

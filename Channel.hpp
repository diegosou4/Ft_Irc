/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 20:54:22 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/29 20:31:35 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// -LIBRARIES-
#include <set>
#include <vector>
#include <iostream>

#include "Client.hpp"
#include "Macros.hpp"

// -CLASS-
class Channel
{
	private:
		std::string	_name;
		std::string _topic;
		std::string _password;
		std::string _modes; //add modes
		std::string _creat; //add creation time

		bool	_invite_only;
		size_t	_limit;

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
		void	setName(std::string const &name);
		void	setTopic(std::string const &topic);
		void	setPassword(std::string const &password);
		void	setLimit(int const &limit);
		void	setModes(char sign, char flag);
		bool	setOperator(std::string target);
		int 	setInvited(std::string const &name);

		//Getters:
		std::string	getName() const;
		std::string	getTopic() const;
		std::string getModes() const;
		std::string getCreat() const;
		std::vector<Client *> getMembers() const;

		//Command-related methods:
		int	addMember(Client &client);
		int	removeMember(Client &client);
		int kickMember(Client &client, std::string const &target);
		int topicHandle(Client &client, std::vector<std::string> const &msg);
		int	modeFlags(Client &client, char sign, char flag, std::string arg);

		//Utils:
		bool	isEmpty() const;
		bool	isMember(Client &client) const;
		bool	isOperator(std::string nick) const;
		bool	needsArg(char sign, char flag);
		Client *findMember(std::string name);
};

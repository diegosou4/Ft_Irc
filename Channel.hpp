/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/17 20:40:47 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/17 21:48:26 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <vector>
#include <iostream>
#include "Client.hpp"
#include <map>
#include <algorithm>

#define PURPLE	"\001\033[1;38;2;209;174;231m\002"
#define GREY	"\001\033[1;37m\002"
#define RED		"\001\033[1;31m\002"
#define R		"\001\033[1;00m\002"

class Client;

class Channel
{
	public:
		Channel(std::string name);
		~Channel();

		std::string joinCmd(Client &client, std::vector<std::string>);
		std::string modeCmd(Client &client, std::vector<std::string>);
		std::string topicCmd(Client &client, std::vector<std::string>);
		std::string inviteCmd(Client &client, std::vector<std::string>);
		std::string privmsgCmd(Client &client, std::vector<std::string>);
		std::string kickCmd(Client &client, std::vector<std::string>);

		std::string _name;
		std::vector<Client *> members;

};

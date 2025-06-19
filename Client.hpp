/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 13:15:34 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/19 14:21:30 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TO BE DONE BY DIEGO

#pragma once

#include <unistd.h>
#include <sstream>
#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>

class Channel;

enum clientState
{
	OFFLINE,
	AT_DOOR,
	PASS_OK,
	NICK_OK,
	ACTIVE
};

class Client
{
	public:
		Client();
		Client(int fd);
		Client(Client const &src);
		~Client();

		Client	&operator=(Client const &src);

		int	fd;
		std::string nickname;
		std::string username;
		std::string realname;
		std::string hostname;
		clientState	state;
};

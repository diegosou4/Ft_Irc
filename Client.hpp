/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 13:15:34 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/24 14:42:58 by feden-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TO BE DONE BY DIEGO

#pragma once

#include <unistd.h>
#include <sstream>
#include <iostream>
#include <netinet/in.h>
#include "includes/Channel.hpp"
#include <arpa/inet.h>

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
	private : 
        int _client_fd;
		std::string _real_name;
		std::string _username;
		std::string _nickname;
		std::string _Ip_address;
		clientState _state;
		std::string	_in_channel;
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

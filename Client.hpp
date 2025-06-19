/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 13:15:34 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/18 22:03:04 by cbouvet          ###   ########.fr       */
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
	AUTH_OK,
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
		void setClientFd(int fd);
		void setRealname(std::string realname);
		void setUsername(std::string username);
		void setInChannel(std::string in_channel);
		void setIpAddress(std::string ip_address);
		void setNickname(std::string nickname);
		void setState(clientState state);
		std::string getIpAddress() const;
		int getClientFd() const;
		clientState getState() const;
		std::string getRealname() const;
		std::string getUsername() const;
		std::string getInChannel() const;
		std::string getNickname() const;
		
};

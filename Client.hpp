/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 16:47:09 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/27 11:29:31 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// -LIBRARIES-
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

// -STRUCTS-
enum clientState
{
	OFFLINE,
	AT_DOOR,
	PASS_OK,
	NICK_OK,
	ACTIVE
};

class Channel;

// -CLASS-
class Client
{
	private:
		int	_client_fd;
		clientState _state;

		std::string _realname;
		std::string _username;
		std::string _nickname;
		std::string _hostname; // more useful than IP - we can get IP from hostname anyways
		std::string _prefix; // needed for compliance with RFC 2812 output responses

	public:
		//Constructor/Destructor:
		Client(); // check if we actually use it?
		Client(int fd);
		Client(Client const &src);
		~Client();

		//Assignement operator:
		Client &operator=(Client const &src);

		//Setters:
		void	setFd(int fd); // shorter to use in Server
		void	setState(clientState state);
		void	setRealname(std::string realname);
		void	setUsername(std::string username);
		void	setNickname(std::string nickname);
		void	setHostname(std::string hostname);
		void	setPrefix();

		//Getters:
		int	getFd() const; // shorter to use in Server
		clientState	getState() const;
		std::string getRealname() const;
		std::string getUsername() const;
		std::string getNickname() const;
		std::string getHostname() const;
		std::string getPrefix() const;
};

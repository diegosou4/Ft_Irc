/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 16:47:09 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/02 13:55:52 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// -LIBRARIES-
#include <ctime>
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
		std::string _hostname;
		std::string _prefix;

		bool _pinged;
		time_t _last_activity;

	public:
		//Constructor/Destructor:
		Client();
		Client(int fd);
		Client(Client const &src);
		~Client();

		//Assignement operator:
		Client &operator=(Client const &src);

		//Setters:
		void	setFd(int fd);
		void	setLastActivity();
		void	setPinged(bool status);
		void	setState(clientState state);
		void	setRealname(std::string realname);
		void	setUsername(std::string username);
		void	setNickname(std::string nickname);
		void	setHostname(std::string hostname);
		void	setPrefix();

		//Getters:
		int	getFd() const;
		time_t getLastActivity()const;
		clientState	getState() const;
		std::string getRealname() const;
		std::string getUsername() const;
		std::string getNickname() const;
		std::string getHostname() const;
		std::string getPrefix() const;

		bool	passedNick()const;
		bool	passedUser()const;
		bool	wasPinged()const;

};

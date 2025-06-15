/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 20:46:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/15 16:16:22 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <poll.h>
#include <stdio.h>
#include <errno.h>
#include <fstream>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "Client.hpp"

#define BUFFSIZE	1000
#define REMOVAL		-2

#ifndef NULL
# define NULL		0
#endif

#define PURPLE	"\001\033[1;38;2;209;174;231m\002"
#define GREY	"\001\033[1;37m\002"
#define RED		"\001\033[1;31m\002"
#define R		"\001\033[1;00m\002"

#define DIGIT_CHAR	"0123456789"
#define ALPHA_CHAR	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define WELCOME \
"\n          ▗▄▄▄▖▗▄▄▖  ▗▄▄▖           \n\
            █  ▐▌ ▐▌▐▌              \n\
            █  ▐▛▀▚▖▐▌              \n\
          ▗▄█▄▖▐▌ ▐▌▝▚▄▄▖           \n\
                                    \n\
▗▖ ▗▖▗▄▄▄▖▗▖    ▗▄▄▖ ▗▄▖ ▗▖  ▗▖▗▄▄▄▖\n\
▐▌ ▐▌▐▌   ▐▌   ▐▌   ▐▌ ▐▌▐▛▚▞▜▌▐▌   \n\
▐▌ ▐▌▐▛▀▀▘▐▌   ▐▌   ▐▌ ▐▌▐▌  ▐▌▐▛▀▀▘\n\
▐▙█▟▌▐▙▄▄▖▐▙▄▄▖▝▚▄▄▖▝▚▄▞▘▐▌  ▐▌▐▙▄▄▖\n\
                                    \n"


class Server
{
	private:
		int	_fd;
		int	_port;
		std::string	_ip;

		bool _active;
		struct sockaddr_in	_addr;
		struct sockaddr *_gen_addr;
		socklen_t		_addrlen;

		std::vector<struct pollfd>	_fds;
		std::map<std::string, Client *> _clients;
		std::map<int, Client *>	_online;

		typedef std::vector<struct pollfd>::iterator pollfd_iter;
		typedef std::map<int, Client *>::iterator online_iter;
		typedef std::map<std::string, Client *>::iterator clients_iter;

		//bool signal?
		//std::string password?
		//std::map<std::string, Channel> _channels?

	public:
		Server(int port, std::string ip); // create fd socket here
		~Server();

		int getFd()const;
		int	getPort()const;
		std::string	getIp()const;
		Client &getClient(int fd);

		void	handleClient(size_t max_fds, int timeout);

		void	removeClient(Client *client);

		bool	isActive();
		bool	hasClient();

	private:
		Server(Server const &src);
		Server	&operator=(Server const &src);

		void	setSocket(in_port_t port, in_addr_t ip);
		void	addSocket(bool isclient);
		void	initServer(int max_fds);

		void	welcomeScreen(struct pollfd &newpollfd);
		void	treatRevent();

		void	pollIn(pollfd_iter &it);
		void	pollErr(pollfd_iter &it);
		void	pollHup(pollfd_iter &it);
		void	pollNVal();

		void	chooseAuth(Client &client);
		void	regUsername(Client &client);
		void	regPass(Client &client);
		void	regPassConfirm(Client &client);
		void	logUsername(Client &client);
		void	logPass(Client &client);
		void	chatMsg(pollfd_iter &it);

		std::string getMsg(Client &client);
		void	broadcast(std::string const &user, std::string const &msg);

		void	clientDataConfig();
		void	clientDataRetrieve();

};

//To add:
// broadcast mechanism for server/wide messages
// buffer overflow protection? Is it really needed tho?
// logging system
// client authentication system
//Add signal handling (e.g., SIGINT) to gracefully shut down.
//Track client metadata: nickname, state, etc.
//Handle POLLERR, POLLHUP, POLLNVAL in treatMsg() or a central poll() result handler.

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 20:46:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/16 17:55:10 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <algorithm>
#include <vector>
#include <map>
#include <poll.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <fstream>
#include <string.h>
#include <csignal>
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
#define ONLINE_OPTS	"\n\n1 - Log in    |    2 - Register"


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
		void	homeScreen(Client &client);
		void	treatRevent();

		void	pollIn(Client &client);
		void	pollErr(pollfd_iter &it);
		void	pollHup(pollfd_iter &it);
		void	pollNVal();

		std::string	chooseAuth(Client &client, std::string msg);
		std::string	regUsername(Client &client, std::string msg);
		std::string	regPass(Client &client, std::string msg);
		std::string	regPassConfirm(Client &client, std::string msg);
		std::string	logUsername(Client &client, std::string msg);
		std::string	logPass(Client &client, std::string msg);
		std::string	chatMsg(Client &client, std::string msg);

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

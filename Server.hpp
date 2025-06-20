/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 20:46:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/20 13:18:07 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// -LIBRARIES-
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
#include "Channel.hpp"

// -MACROS-
//General:
#ifndef NULL
# define NULL		0
#endif
//Socket setup related:
#define LOCALHOST	"127.0.0.1"
#define BUFFSIZE	1000
#define REMOVAL		-2
//Colours:
#define PURPLE	"\001\033[1;38;2;209;174;231m\002"
#define GREY	"\001\033[1;37m\002"
#define RED		"\001\033[1;31m\002"
#define R		"\001\033[1;00m\002"
//Output messages:
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
#define ONLINE_OPTS		"\n\n1 - Log in    |    2 - Register"
#define INSTRUCTIONS	"Please enter using PASS"
#define PASS_EXPECT		"Expected: PASS <password>"
#define NICK_EXPECT		"Expected: NICK <nickname>"
#define USER_EXPECT		"Expected: USER <username> <hostname> <servername> :<realname>"
#define JOIN_EXPECT		"Expected: JOIN <#channelname>"

// -CLASS-
class Server
{
	private:
		// Typedefs:
		typedef std::vector<struct pollfd>::iterator pollfd_iter;
		typedef std::map<std::string, Client *>::iterator clients_iter;
		typedef std::map<std::string, Channel *>::iterator channels_iter;
		typedef std::string (Server::*AuthCmds)(Client*, Channel *, std::vector<std::string>&);
		typedef std::string (Channel::*ChanCmds)(Client&, std::vector<std::string>&);

		// Regular attributes:
		int	_fd;
		int	_port;
		std::string	_password;

		struct sockaddr_in	_addr;
		struct sockaddr *_gen_addr;
		socklen_t		_addrlen;

		// Container attributes:
		std::vector<struct pollfd>	_fds;
		std::map<std::string, Client *> _clients;
		std::map<std::string, Channel *> _channels;
		std::map<std::string, AuthCmds> _authcmds;
		std::map<std::string, ChanCmds> _chancmds;

	public:
		// Constructors/Destructors - Server.cpp
		Server(int port, std::string password);
		~Server();

		// Getters/Setters - Server.cpp
		int getFd()const;
		int	getPort()const;
		Client *getClient(int fd);

		// Init/setup - Server.cpp
		void	handleClient(size_t max_fds, int timeout);
		void	removeClient(Client &client);

	private:
		// Forbidden methods - Server.cpp
		Server(Server const &src);
		Server	&operator=(Server const &src);

		// Init/setup - Server.cpp
		void	setSocket(in_port_t port, in_addr_t ip);
		void	initServer(int max_fds);
		void	addSocket(bool isclient);

		// Poll/revents - Revents.cpp
		void	treatRevent();
		void	pollIn(Client &client);
		void	pollErr(Client &client);
		void	pollHup(Client &client);
		void	pollNVal(Client &client);

		// Commands - Commands.cpp
		std::string passCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		std::string nickCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		std::string userCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		std::string joinCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg);

		std::string passCmd(Client &client, std::string password);
		std::string nickCmd(Client *client, std::string nickname);
		std::string userCmd(Client &client, std::vector<std::string> &user_args);
		std::string joinCmd(Client &client, Channel *channel, std::string channelname);

		// Utils - Utils.cpp
		void	setCmdMaps();
		void	welcomeScreen(Client &client);
		std::string getMsg(Client &client);
		std::vector<std::string> splitMsg(std::string &msg);
		Channel	*findChannel(std::vector<std::string> &split_msg);
		void	broadcast(Client &client, Channel *channel, std::string const &msg);
};


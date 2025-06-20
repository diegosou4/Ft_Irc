/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 20:46:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/20 10:28:28 by cbouvet          ###   ########.fr       */
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
#include "Channel.hpp"

#define LOCALHOST	"127.0.0.1"
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
#define INSTRUCTIONS "Please enter using PASS and NICK"

enum CmdsEnum
{
	INVALID,
	PASS,
	NICK,
	USER,
	JOIN,
	MODE,
	TOPIC,
	INVITE,
	PRIVMSG,
	KICK
};
/*
PASS secretpassword
NICK mynickname
USER myuser 0 * :Real Name Here
JOIN #mychannel
MODE #mychannel +o username
TOPIC #mychannel :This is the new topic
INVITE username #mychannel
PRIVMSG #mychannel :Hello, world!
KICK #mychannel username :optional reason*/

class Server
{
	private:
		int	_fd;
		int	_port;
		std::string	_password;

		struct sockaddr_in	_addr;
		struct sockaddr *_gen_addr;
		socklen_t		_addrlen;

		std::vector<struct pollfd>	_fds;
		std::map<std::string, Client *> _clients;
		std::map<std::string, Channel *> _channels;
		std::map<std::string, CmdsEnum> cmdMap;

		typedef std::vector<struct pollfd>::iterator pollfd_iter;
		typedef std::map<std::string, Client *>::iterator clients_iter;
		typedef std::map<std::string, Channel *>::iterator channels_iter;

		typedef std::string (Server::*AuthCmds)(Client*, Channel *, std::vector<std::string>&);
		typedef std::string (Channel::*ChanCmds)(Client&, std::vector<std::string>&);

		std::map<std::string, AuthCmds> _authcmds;
		std::map<std::string, ChanCmds> _chancmds;

	public:
	//----------------------Constructors/Destructors-------------------------------
		Server(int port, std::string password);
		~Server();

		//---------------------------Getters/Setters-----------------------------------
		int getFd()const;
		int	getPort()const;
		Client *getClient(int fd);

		//-----------------------------Init/setup--------------------------------------
		void	handleClient(size_t max_fds, int timeout);
		void	removeClient(Client &client);

	private:
	//----------------------Constructors/Destructors-------------------------------
		Server(Server const &src);
		//-------------------------Operator overloads----------------------------------
		Server	&operator=(Server const &src);

		//-----------------------------Init/setup--------------------------------------
		void	setSocket(in_port_t port, in_addr_t ip);
		void	initServer(int max_fds);
		void	addSocket(bool isclient);

		//----------------------------Poll/revents-------------------------------------
		void	treatRevent();
		void	pollIn(Client &client);
		void	pollErr(Client &client);
		void	pollHup(Client &client);
		void	pollNVal(Client &client);

		//------------------------------Commands---------------------------------------
		void	authCmds(std::vector<std::string> &split_msg, CmdsEnum cmd, Client &client);
		void	channelCmds(std::vector<std::string> &split_msg, CmdsEnum cmd, Client &client);

		/* std::string passCmd(Client &client, std::vector<std::string> &split_msg);
		std::string nickCmd(Client *client, std::vector<std::string> &split_msg);
		std::string userCmd(Client &client, std::vector<std::string> &split_msg); */

		std::string passCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		std::string nickCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		std::string userCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		std::string joinCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		//--------------------------------Utils----------------------------------------
		void	setCmdMap();
		void	welcomeScreen(Client &client);
		std::string getMsg(Client &client);
		std::vector<std::string> splitMsg(std::string msg);
		void	broadcast(Client &client, Channel *channel, std::string const &msg);
};


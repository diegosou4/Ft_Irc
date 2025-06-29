/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 20:46:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/29 16:44:38 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// -LIBRARIES-
#include "Macros.hpp"
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
#include <sstream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <iomanip>
#include "Client.hpp"
#include "Channel.hpp"
#include <cstring>

// -CLASS-
class Server
{
	private:
		// Typedefs:
		typedef std::vector<std::string> str_vector;
		typedef std::vector<struct pollfd>::iterator pollfd_iter;
		typedef std::map<std::string, Client *>::iterator clients_iter;
		typedef std::map<std::string, Channel *>::iterator channels_iter;
		typedef void (Server::*AuthCmds)(Client*, Channel *, str_vector const&);

		// Regular attributes:
		int	_fd;
		int	_port;
		std::string _name;
		std::string	_password;

		struct sockaddr_in	_addr;
		struct sockaddr *_gen_addr;
		socklen_t		_addrlen;

		// Container attributes:
		std::vector<struct pollfd>	_fds;
		std::map<std::string, Client *> _clients;
		std::map<std::string, Channel *> _channels;
		std::map<std::string, AuthCmds> _authcmds;

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
		void cmdPass(Client *client, Channel *channel, str_vector const &msg);
		void cmdNick(Client *client, Channel *channel, str_vector const &msg);
		void cmdUser(Client *client, Channel *channel, str_vector const &msg);
		void cmdJoin(Client *client, Channel *channel, str_vector const &msg);
		void cmdInvite(Client *client, Channel *channel, str_vector const &msg);
		void cmdKick(Client *client, Channel *channel, str_vector const &msg);
		void cmdPart(Client *client, Channel *channel, str_vector const &msg);
		void cmdQuit(Client *client, Channel *channel, str_vector const &msg);
		void cmdNames(Client *client, Channel *channel, str_vector const &msg);
		void cmdPrivmsg(Client *client, Channel *channel, str_vector const &msg);
		void cmdTopic(Client *client, Channel *channel, str_vector const &msg);
		void cmdMode(Client *client, Channel *channel, str_vector const &msg);

		// Utils - Utils.cpp
		void	setCmdMaps();
		std::string getMsg(Client &client);
		str_vector splitMsg(std::string &msg);
		Channel	*findChannel(str_vector &split_msg);

		void	printServer(Client *client, std::string const &msg);
		void	sendClient(Client &client, std::string const &msg);
		void	sendNumeric(Client &client, int code);
		void	sendNumeric(Client &client, int code, std::string const &msg);
		void	broadcast(Client &client, std::string const &cmd, std::string const &msg);
		void	broadcast(Client &client, Channel &channel, std::string const &cmd, std::string const &msg);
		void	broadcast(Client &client, Client &target, std::string const &cmd, std::string const &msg);
		bool	authCheck(Client &client);
		int		cmdCheck(Client *client, Channel *channel, std::string target);
		int		checkModeFormat(str_vector const &msg);
		void	sendMode(Client &client, Channel &channel, int stop, str_vector const &msg);
		std::string nameList(Channel &channel);
		str_vector newVector(std::string const &arg1, std::string const &arg2);
		std::string	unSplit(str_vector const &msg, int index);
	};


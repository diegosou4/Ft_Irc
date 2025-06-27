/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 20:46:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/27 20:44:58 by cbouvet          ###   ########.fr       */
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

// -CLASS-
class Server
{
	private:
		// Typedefs:
		typedef std::vector<struct pollfd>::iterator pollfd_iter;
		typedef std::map<std::string, Client *>::iterator clients_iter;
		typedef std::map<std::string, Channel *>::iterator channels_iter;
		typedef int (Server::*AuthCmds)(Client*, Channel *, std::vector<std::string>&);

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
		int passCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		int nickCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		int userCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		int joinCheck(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		int privMsgCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		int modeCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		int topicCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		int inviteCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		int privmsgCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg);
		int kickCmd(Client *client, Channel *channel, std::vector<std::string> &split_msg);

		int passCmd(Client &client, std::string password);
		int nickCmd(Client *client, std::string nickname);
		int userCmd(Client &client, std::vector<std::string> &user_args);
		int joinCmd(Client &client, Channel *channel, std::string channelname);

		// Utils - Utils.cpp
		void	setCmdMaps();
		std::string getMsg(Client &client);
		std::vector<std::string> splitMsg(std::string &msg);
		Channel	*findChannel(std::vector<std::string> &split_msg);

		void	printServer(Client *client, std::string const &msg);
		void	sendClient(Client &client, std::string const &msg);
		void	sendNumeric(Client &client, int code);
		void	sendNumeric(Client &client, int code, std::string const &msg);
		void	broadcast(Client &client, Channel &channel, std::string &cmd, std::string const &msg)
};


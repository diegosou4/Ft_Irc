/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 20:46:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/05 16:43:35 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

// -LIBRARIES-
#include <map>
#include <ctime>
#include <vector>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <algorithm>

#include <poll.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "Client.hpp"
#include "Macros.hpp"
#include "Channel.hpp"

// -CLASS-
class Server
{
	private:
		// Typedefs:
		typedef std::vector<std::string> str_vector;
		typedef void (Server::*AuthCmds)(Client*, Channel *, str_vector const&);

		typedef std::vector<struct pollfd>::iterator pollfd_iter;
		typedef std::map<std::string, Client *>::iterator clients_iter;
		typedef std::map<std::string, Channel *>::iterator channels_iter;

		// Regular attributes:
		int	_fd;
		int	_port;
		std::string _name;
		std::string	_password;

		// Struct attributes:
		socklen_t _addrlen;
		struct sockaddr_in _addr;
		struct sockaddr *_gen_addr;

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
		void handleClient(size_t max_fds, int timeout);
		void removeClient(Client *client);

	private:
		// Forbidden methods - Server.cpp
		Server(Server const &src);
		Server &operator=(Server const &src);

		// Init/setup - Server.cpp
		void setSocket(in_port_t port, in_addr_t ip);
		void initServer(int max_fds);
		void addSocket(bool isclient);

		// Poll/revents - Revents.cpp
		void treatRevent();
		void pollIn(Client &client);
		void pollErr(Client &client);
		void pollHup(Client &client);
		void pollNVal(Client &client);

		// Messages senders - MsgSenders.cpp
		void printServer(Client *client, std::string const &msg);
		void sendClient(Client &client, std::string const &msg);
		void sendNumeric(Client &client, Channel *channel, int code);
		void sendNumeric(Client &client, int code, std::string const &msg);
		void broadcast(Client &client, Client &target, std::string const &cmd, std::string const &msg);
		void broadcastAll(Client &client, Channel &channel, std::string const &cmd, std::string const &msg);
		void broadcastOthers(Client &client, Channel &channel, std::string const &cmd, std::string const &msg);

		// Authentication-related Commands - AuthCmds.cpp
		void cmdPass(Client *client, Channel *channel, str_vector const &msg);
		void cmdNick(Client *client, Channel *channel, str_vector const &msg);
		void cmdUser(Client *client, Channel *channel, str_vector const &msg);
		void cmdQuit(Client *client, Channel *channel, str_vector const &msg);

		// Channel-related Commands - ChanCmds.cpp
		void cmdJoin(Client *client, Channel *channel, str_vector const &msg);
		void cmdInvite(Client *client, Channel *channel, str_vector const &msg);
		void cmdKick(Client *client, Channel *channel, str_vector const &msg);
		void cmdPart(Client *client, Channel *channel, str_vector const &msg);
		void cmdNames(Client *client, Channel *channel, str_vector const &msg);
		void cmdPrivmsg(Client *client, Channel *channel, str_vector const &msg);
		void cmdNotice(Client *client, Channel *channel, str_vector const &msg);
		void cmdTopic(Client *client, Channel *channel, str_vector const &msg);
		void cmdMode(Client *client, Channel *channel, str_vector const &msg);

		// Command-related Utils = CmdUtils.cpp
		Channel *findChannel(str_vector &split_msg);
		std::string nameList(Channel &channel);
		int	cmdCheck(Client *client, Channel *channel, std::string target);
		int	checkModeFormat(str_vector const &msg);
		void sendMode(Client &client, Channel &channel, size_t stop, str_vector const &msg);

		// Utils - Utils.cpp
		void setCmdMaps();
		int parseNick(const std::string &nick);
		bool authCheck(Client &client);
		void leaveAllChans(std::string cmd, Client &client, std::string msg);
		std::string getMsg(Client &client);
		std::string argExists(str_vector const &msg, size_t index);
		str_vector splitMsg(std::string &msg);
		str_vector newVector(std::string const &arg1, std::string const &arg2, std::string *arg3);
	};


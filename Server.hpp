/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/02 20:46:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/05 11:46:04 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include "classes.hpp"

#define PURPLE	"\001\033[1;38;2;209;174;231m\002"
#define BLUE	"\001\033[1;38;2;147;222;255m\002"
#define GREY	"\001\033[1;37m\002"
#define RED		"\001\033[1;31m\002"
#define R		"\001\033[1;00m\002"

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
		std::map<int, Client>	_clients;

		typedef std::vector<struct pollfd>::iterator pollfd_iter;

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

		void	initServer(int max_fds);
		void	readClient(size_t max_fds, int timeout);
		void	treatMsg(int buffsize);

		void	removeClient(pollfd_iter it);

		bool	isActive();
		bool	hasClient();

	private:
		Server(Server const &src);
		Server	&operator=(Server const &src);

		void	setSocket(in_port_t port, in_addr_t ip);
		void	pollErr();
		void	pollUp();

		void	broadcast(std::string &msg);
		void	acceptClient(int fd); //unsure
};

//To add:
// broadcast mechanism for server/wide messages
// buffer overflow protection? Is it really needed tho?
// logging system
// client authentication system
//Add signal handling (e.g., SIGINT) to gracefully shut down.
//Track client metadata: nickname, state, etc.
//Handle POLLERR, POLLHUP, POLLNVAL in treatMsg() or a central poll() result handler.

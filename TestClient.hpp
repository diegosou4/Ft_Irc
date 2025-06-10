/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TestClient.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 13:15:44 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/10 15:34:15 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <poll.h>

#define PURPLE	"\001\033[1;38;2;209;174;231m\002"
#define GREY	"\001\033[1;37m\002"
#define RED		"\001\033[1;31m\002"
#define R		"\001\033[1;00m\002"

class TestClient
{
	private:
		int	_fd;
		int	_port;
		std::string	_ip;

		bool _connected;
		struct sockaddr_in	_addr;
		struct sockaddr *_gen_addr;
		socklen_t		_addrlen;

	public:
		TestClient(int port, std::string ip);
		~TestClient();

		int getFd()const;
		int getPort()const;
		std::string getIp()const;

		void connectClient();
		void getOnline(int buffsize);

	private:
		TestClient(TestClient const &src);
		TestClient	&operator=(TestClient const &src);

		void setSocket(in_port_t port, in_addr_t ip);
		struct pollfd setPollfd(int fd);
		void	receiveMsg(int buffsize);
		void	sendMsg();
};

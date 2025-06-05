/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   classes.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 21:58:18 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/05 11:28:25 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <poll.h>

# define GREY	"\001\033[1;37m\002"
#define RED	"\001\033[1;31m\002"
# define R		"\001\033[1;00m\002"

class Client
{
	public:
		Client();
		Client(int fd);
		~Client(); // make it close here

		void	setConnect(sa_family_t family, std::string ip, int port);
		int fd;
		struct sockaddr_in	addr;
		struct sockaddr		*gen_addr;
		socklen_t			addrlen;
		//Personal data to add:
		/* std::string name;
		std::string nickname;
		std::string password; */
		//attribute for which channels it's in?
};

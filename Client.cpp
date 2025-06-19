/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 13:15:41 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/19 14:34:41 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TO BE DONE BY DIEGO

#include "Client.hpp"
#include "Channel.hpp"


Client::Client(): fd(-1), state(OFFLINE)
{
	char hostname[1000];

	if (!gethostname(hostname, 1000))
		this->nickname = "user@" + std::string(hostname);
	else
		this->nickname = "user@unknown";
}

Client::Client(int fd): fd(fd), state(OFFLINE)
{
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	if (!getpeername(fd, (struct sockaddr *)&addr, &addrlen))
		this->nickname = "user@" + std::string(inet_ntoa(addr.sin_addr));
	else
		this->nickname = "user@unknown";
}

Client::Client(Client const &src)
{
	this->fd = src.fd;
	this->state = src.state;
	this->nickname = src.nickname;
	this->username = src.username;
}

Client::~Client()
{
}

Client	&Client::operator=(Client const &src)
{
	if (this != &src)
	{
		//this->fd = src.fd;
		this->state = src.state;
		this->nickname = src.nickname;
		this->username = src.username;
	}

	return (*this);
}



/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 13:15:41 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/17 21:40:10 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TO BE DONE BY DIEGO

#include "Client.hpp"
#include "Channel.hpp"


Client::Client(): fd(-1), username("anyonymous"), state(OFFLINE), in_channel("")
{
	//std::cout << "I'm being built up with no fd" << std::endl;
}

Client::Client(int fd): fd(fd), state(OFFLINE), in_channel("")
{
	std::stringstream ss;
	ss << "Client " << fd;

	this->username = ss.str();
}

Client::Client(Client const &src)
{
	this->fd = src.fd;
	this->state = src.state;
	this->username = src.username;
	this->in_channel = src.in_channel;
}

Client::~Client()
{
}

Client	&Client::operator=(Client const &src)
{
	if (this != &src)
	{
		this->fd = src.fd;
		this->state = src.state;
		this->username = src.username;
	}

	return (*this);
}



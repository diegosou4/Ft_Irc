/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   classes.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/29 10:44:44 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/04 17:41:06 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "classes.hpp"

Client::Client(int fd): fd(fd)
{
	if (this->fd < 0)
		throw (std::runtime_error("Client socket creation failed"));

	std::cout << GREY "Created client with open fd " << this->fd << R << std::endl;
}

Client::~Client()
{
	std::cout << "Disconnecting" << std::endl;
	if (this->fd >= 0)
		close(this->fd);
}

void	Client::setConnect(sa_family_t family, std::string ip, int port)
{
	this->addr.sin_family = family;
	this->addr.sin_addr.s_addr = inet_addr(ip.c_str());
	this->addr.sin_port = htons(port);

	this->gen_addr = (struct sockaddr *)&this->addr;
	this->addrlen = sizeof(this->addr);
}

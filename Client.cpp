/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 13:15:41 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/12 12:21:15 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TO BE DONE BY DIEGO

#include "Client.hpp"


Client::Client(): fd(-1), username("anyonymous")
{
	//std::cout << "I'm being built up with no fd" << std::endl;
}

Client::Client(int fd): fd(fd)
{
	std::stringstream ss;
	ss << "Client " << fd;

	this->username = ss.str();
}

Client::~Client()
{
}



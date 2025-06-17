/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 13:15:34 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/17 21:39:50 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TO BE DONE BY DIEGO

#pragma once

#include <unistd.h>
#include <sstream>
#include <iostream>

class Channel;

enum clientState
{
	ONLINE,
	ACTIVE,
	OFFLINE
};

class Client
{
	public:
		Client();
		Client(int fd);
		Client(Client const &src);
		~Client();

		Client	&operator=(Client const &src);

		int	fd;
		std::string nickname;
		std::string username;
		clientState	state;
		std::string	in_channel;
};

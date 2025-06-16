/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 13:15:34 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/16 18:05:34 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TO BE DONE BY DIEGO

#pragma once

#include <unistd.h>
#include <sstream>
#include <iostream>

enum clientState
{
	ONLINE,
	REG_USERNAME,
	REG_PASS,
	REG_PASSCONFIRM,
	LOG_USERNAME,
	LOG_PASS,
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
		std::string username;
		std::string password;
		clientState	state;
};

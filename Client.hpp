/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 13:15:34 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/12 11:22:29 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TO BE DONE BY DIEGO

#pragma once

#include <unistd.h>
#include <sstream>
#include <iostream>

class Client
{
	public:
		Client();
		Client(int fd);
		~Client();

		int	fd;
		std::string username;
		std::string password;
};

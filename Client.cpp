/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 13:15:41 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/08 10:49:27 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TO BE DONE BY DIEGO

#include "Client.hpp"
#include <iostream>

Client::Client(): fd(-1)
{
	std::cout << "I'm being built up with no fd" << std::endl;
}

Client::Client(int fd): fd(fd)
{
	std::cout << "I'm being built up with fd" << std::endl;
}

Client::~Client()
{
	std::cout << "I'm closing now" << std::endl;
}

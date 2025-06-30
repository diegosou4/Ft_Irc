/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 22:11:55 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/19 22:51:34 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <csignal>
#include <stdlib.h>
#include "TestClient.hpp"

#define RED		"\001\033[1;31m\002"
#define R		"\001\033[1;00m\002"

#define ARG_EXPECT	"Expected: ./test_client <port>\n"
#define LOCALHOST	"127.0.0.1"
#define DIGIT_CHARS	"0123456789"

void	sigint_handler(int signal);
int		port_check(std::string port);

int	main (int ac, char **av)
{
	std::signal(SIGINT, sigint_handler);

	try
	{
		if (ac != 2)
			throw (std::runtime_error("Wrong number of params\n" R + std::string(ARG_EXPECT)));

		int port = port_check(std::string(av[1]));

		TestClient client(port, LOCALHOST);
		client.connectClient();
		client.getOnline();
	}
	catch(const std::exception& e)
	{
		std::cerr << RED << e.what() << R << std::endl;
	}

}

void	sigint_handler(int signal)
{
	(void)signal;
	throw (std::runtime_error("\nCtrl-C intercepted - exiting programme"));
}

int		port_check(std::string port)
{
	if (port.empty() || port.length() != 4
	|| port.find_first_not_of(DIGIT_CHARS) != port.npos)
		throw (std::runtime_error("Invalid param format\n" R + std::string(ARG_EXPECT)));

	int int_port = atoi(port.c_str());
	return (int_port);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 21:57:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/19 22:51:44 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <iostream>
#include <csignal>

#define BLUE	"\001\033[1;38;2;147;222;255m\002"
#define RED		"\001\033[1;31m\002"
#define R		"\001\033[1;00m\002"

#define ARG_EXPECT	"Expected: ./test_client <port> <password>\n"
#define DIGIT_CHARS	"0123456789"

#define MAX_CLIENTS	10
#define TIMEOUT		-1

void	sigint_handler(int signal);
int		port_check(std::string port);

int main(int ac, char **av)
{
	std::signal(SIGINT, sigint_handler);

	try
	{
		if (ac != 3)
			throw (std::runtime_error("Wrong number of params\n" R + std::string(ARG_EXPECT)));

		int port = port_check(std::string(av[1]));
		if (!av[2][0])
			throw (std::runtime_error("Invalid password param\n" R + std::string(ARG_EXPECT)));

		Server server(port, std::string(av[2]));
		server.handleClient(MAX_CLIENTS, TIMEOUT);
	}
	catch (std::exception &e)
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
		throw (std::runtime_error("Invalid port param\n" R + std::string(ARG_EXPECT)));

	int int_port = atoi(port.c_str());
	return (int_port);
}

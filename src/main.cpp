/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 21:57:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/06 10:15:55 by feden-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// -LIBRARIES-
#include "../includes/Server.hpp"
#include <iostream>
#include <csignal>

// -MACROS-
//Colours:
#define BLUE	"\001\033[1;38;2;147;222;255m\002"
#define RED		"\001\033[1;31m\002"
#define R		"\001\033[1;00m\002"
//Arg-checking:
#define ARG_EXPECT	"Expected: ./test_client <port> <password>\n"

#ifndef DIGIT_CHARS
 #define DIGIT_CHARS	"0123456789"
#endif

//Set values:
#define MAX_CLIENTS	10
#define TIMEOUT		-1

// -FUNCTION DEFINITION-
void	sigint_handler(int signal);
int		port_check(std::string port);

// -FUNCTIONS-
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

// Throws an exception for clean exit upon Ctrl-C trigger
void	sigint_handler(int signal)
{
	(void)signal;
	throw (std::runtime_error("\nCtrl-C intercepted - exiting programme"));
}

// Checks validity of port format
int		port_check(std::string port)
{
	if (port.empty() || port.length() != 4
	|| port.find_first_not_of(DIGIT_CHARS) != port.npos)
		throw (std::runtime_error("Invalid port param\n" R + std::string(ARG_EXPECT)));

	int int_port = atoi(port.c_str());
	return (int_port);
}

/*
PASS secretpassword
NICK mynickname
USER myuser 0 * :Real Name Here
JOIN #mychannel
MODE #mychannel +o username
TOPIC #mychannel :This is the new topic
INVITE username #mychannel
PRIVMSG #mychannel :Hello, world!
KICK #mychannel username :optional reason*/

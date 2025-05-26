/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 21:57:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/05/26 23:08:25 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "my_irc.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

# define RED	"\001\033[1;31m\002"
# define R		"\001\033[1;00m\002"

void	server();
void	client();

int main(int ac, char **av)
{
	try
	{
		if (ac != 2)
			throw (std::runtime_error("Arg number wrong"));
		if (std::string(av[1]) == "client")
			client();
		else if (std::string(av[1]) == "server")
			server();
		else
			throw (std::runtime_error("Wrong arg"));
	}
	catch (std::exception &e)
	{
		std::cerr << RED << e.what() << R << std::endl;
	}

}

void	server()
{
	//Initiate port
	int port = 4242;

	// Create socket
	int server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		throw (std::runtime_error("Server socket creation failed"));

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("localhost");
	address.sin_port = htons(port);

	(void)address;
	(void)server_fd;
	//bind host to port
	//determine how many clients can the server listen to at the same time

	//accept new connection
	//loop to receive data
		//print whatever's received to stdout
		//send data to client
	//close connection
}

void	client()
{
	//initiate port
	int port = 4242;

	//instantiate socket
	int client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0)
		throw (std::runtime_error("Client socket creation failed"));

	//connect to server
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(port);

	(void)address;
	(void)client_fd;

	//take input
	//loop till
		//send message to socket
		//receive response
		//print to stdout
	//close connection
}

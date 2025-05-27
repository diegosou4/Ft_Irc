/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 21:57:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/05/27 18:00:02 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "my_irc.hpp"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <climits>
#include <unistd.h>
#include <cstring>

# define PURPLE	"\001\033[1;38;2;209;174;231m\002"
# define GREY	"\001\033[1;37m\002"
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
	std::cout << GREY "Acting as server" R << std::endl;

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

	struct sockaddr *simple_addr = (struct sockaddr *)&address;

	//bind host to port
	if (bind(server_fd, simple_addr, sizeof(address)) < 0)
		throw (std::runtime_error("Server binding failed"));

	//determine how many clients can the server listen to at the same time
	if (listen(server_fd, 10) < 0)
		throw (std::runtime_error("Server fails to listen"));

	//accept new connection
	int connected_socket = accept(server_fd, simple_addr, (socklen_t *)sizeof(address));
	if (connected_socket < 0)
		throw (std::runtime_error("Server failed to accept connection"));

	//loop to receive data
	void *buff;
	while (true)
	{
		memset(buff, 0, sizeof(&buff));
		int bytes_read = recv(connected_socket, buff, INT_MAX, 0);
		if (bytes_read && !strcmp((char *)buff, "exit"))
			break;
		if (bytes_read)
		{
			std::cout << PURPLE "> Client: " R << buff << std::endl;
			send(connected_socket, buff, bytes_read, 0);
		}
	}

	//close connection
	close(connected_socket);
	close(server_fd);
}

void	client()
{
	std::cout << GREY "Connecting as client" R << std::endl;

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

	struct sockaddr *simple_addr = (struct sockaddr *)&address;

	//Connect to server
	if (connect(client_fd, simple_addr, sizeof(address)) < 0)
		throw (std::runtime_error("Client failed to connect to server"));

	//take input + send
	char buff[1000];
	while (true)
	{
		std::cout << "> ";
		std::string input;
		getline(std::cin, input);

		if (!input.empty())
		{
			send(client_fd, input.c_str(), input.size(), 0);
			int bytes = recv(client_fd, buff, 1000, 0);
			if (bytes)
				std::cout << buff << std::endl;
			memset(buff, 0, bytes);
		}

		if (input == "exit")
			break;
	}

	//close connection
	close(client_fd);
}

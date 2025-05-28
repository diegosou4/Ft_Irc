/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 21:57:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/05/28 21:11:07 by cbouvet          ###   ########.fr       */
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
#include <poll.h>

# define PURPLE	"\001\033[1;38;2;209;174;231m\002"
# define GREY	"\001\033[1;37m\002"
# define RED	"\001\033[1;31m\002"
# define R		"\001\033[1;00m\002"

void	server(int &server_fd);
void	client(int &client_fd);

int main(int ac, char **av)
{
	int server_fd = -1;
	int client_fd = -1;
	try
	{
		if (ac != 2)
			throw (std::runtime_error("Arg number wrong"));
		if (std::string(av[1]) == "client")
			client(client_fd);
		else if (std::string(av[1]) == "server")
			server(server_fd);
		else
			throw (std::runtime_error("Wrong arg"));
	}
	catch (std::exception &e)
	{
		if (server_fd > 0)
			close(server_fd);
		if (client_fd > 0)
			close(client_fd);
		std::cerr << RED << e.what() << R << std::endl;
	}

}

void	server(int &server_fd)
{
	std::cout << GREY "Acting as server" R << std::endl;

	//Initiate port
	int port = 4242;

	// Create socket
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd < 0)
		throw (std::runtime_error("Server socket creation failed"));

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(port);

	struct sockaddr *simple_addr = (struct sockaddr *)&address;
	socklen_t addrlen = sizeof(address);

	//Remove timeout in case of unexpected exit
	int opt = 1;
	setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	//bind host to port
	if (bind(server_fd, simple_addr, addrlen) < 0)
		throw (std::runtime_error("Server binding failed"));

	//determine how many clients can the server listen to at the same time
	if (listen(server_fd, 10) < 0)
		throw (std::runtime_error("Server fails to listen"));

	//prepare poll structure
	struct pollfd connected_sockets[42];
	int slots_taken = 1;
	connected_sockets[0].fd = server_fd;
	connected_sockets[0].events = POLLIN;


	//accept new connection
	while (true)
	{
		if (poll(connected_sockets, 42, -1) < 0)
			throw (std::runtime_error("Poll failed"));

		if (connected_sockets[0].revents & POLLIN)
		{
			slots_taken++;
			connected_sockets[slots_taken -1].fd = accept(server_fd, simple_addr, &addrlen);

			if (connected_sockets[slots_taken -1].fd < 0)
				throw (std::runtime_error("Server failed to accept connection"));
			if (slots_taken >= 41)
			{
				close(connected_sockets[slots_taken -1].fd);
				std::cout << RED "Max clients reached" R << std::endl;
			}
			else
			{
				connected_sockets[slots_taken -1].events = POLLIN;
				std::cout << GREY "New client connected" R << std::endl;
			}
		}

		for (int i = 1; i < slots_taken; i++)
		{
			if (connected_sockets[i].revents & POLLIN)
			{
				std::cout << "SEEN" << std::endl;
				char buff[1000];
				memset(buff, 0, 1000);
				int bytes_read = recv(connected_sockets[i].fd, buff, 1000, 0);
				if (bytes_read <=0 || strcmp(buff, "exit") == 0)
				{
					close(connected_sockets[i].fd);
					std::cout << PURPLE "> Client " << i << " has left" R << std::endl;
					for (int j = i; j < slots_taken -1; j++)
						connected_sockets[j] = connected_sockets[j +1];
					slots_taken--;
					i--;
				}
				else
				{
					std::cout << PURPLE "> Client " << i << ": " R << buff << std::endl;
					send(connected_sockets[i].fd, buff, bytes_read, 0);
				}
			}
		}
	}
	//close connection
	close(server_fd);
}

void	client(int &client_fd)
{
	std::cout << GREY "Connecting as client" R << std::endl;

	//initiate port
	int port = 4242;

	//instantiate socket
	client_fd = socket(AF_INET, SOCK_STREAM, 0);
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
				std::cout << GREY "sent" R << std::endl;
			memset(buff, 0, bytes);
		}

		if (input == "exit")
			break;
	}

	//close connection
	close(client_fd);
}

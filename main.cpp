/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 21:57:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/05/29 22:56:09 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "classes.hpp"
#include <cstring>

#define PURPLE	"\001\033[1;38;2;209;174;231m\002"
#define BLUE	"\001\033[1;38;2;147;222;255m\002"
#define GREY	"\001\033[1;37m\002"
#define RED	"\001\033[1;31m\002"
#define R		"\001\033[1;00m\002"

#define PORT		4242
#define LOCALHOST	"127.0.0.1"
#define BUFFSIZE	1000
#define MAX_CLIENTS	10
#define TIMEOUT		-1

void	server(int &server_fd);
void	client();

int main(int ac, char **av)
{
	int server_fd = -1;
	try
	{
		if (ac != 2)
			throw (std::runtime_error("Arg number wrong"));
		if (std::string(av[1]) == "client")
			client();
		else if (std::string(av[1]) == "server")
			server(server_fd);
		else
			throw (std::runtime_error("Wrong arg"));
	}
	catch (std::exception &e)
	{
		if (server_fd > 0)
			close(server_fd);
		std::cerr << RED << e.what() << R << std::endl;
	}

}

void	server(int &server_fd)
{
	std::cout << BLUE "Acting as server" R << std::endl;

	Client machine(socket(AF_INET, SOCK_STREAM, 0));
	machine.setConnect(AF_INET, LOCALHOST, PORT);

	Server server(machine, TIMEOUT);
	server.setServer(MAX_CLIENTS);

	while (true)
	{
		server.getInput(MAX_CLIENTS, TIMEOUT);

		for (int i = 1; i < server.openfds; i++)
		{
			if (server.fds[i].revents & POLLIN)
			{
				char buff[BUFFSIZE];
				memset(buff, 0, BUFFSIZE);
				int bytes_read = recv(server.fds[i].fd, buff, BUFFSIZE, 0);
				if (bytes_read <=0 || strcmp(buff, "exit") == 0)
				{
					close(server.fds[i].fd);
					std::cout << PURPLE "> Client " << i << " has left" R << std::endl;
					for (int j = i; j < server.openfds -1; j++)
						server.fds[j] = server.fds[j +1];
					server.openfds--;
					i--;
				}
				else
				{
					std::cout << PURPLE "> Client " << i << ": " R << buff << std::endl;
					send(server.fds[i].fd, buff, bytes_read, 0);
				}
			}
		}
	}

	close(server_fd);
}

void	client()
{
	std::cout << BLUE "Selected: client connection" R << std::endl;

	//instantiate sclient
	Client	client(socket(AF_INET, SOCK_STREAM, 0));
	client.setConnect(AF_INET, LOCALHOST, PORT);

	//Connect to server
	if (connect(client.fd, client.gen_addr, sizeof(client.addr)) < 0)
		throw (std::runtime_error("Client failed to connect to server"));

	//take input + send
	char buff[BUFFSIZE];
	while (true)
	{
		std::cout << "> ";
		std::string input;
		getline(std::cin, input);

		if (!input.empty())
		{
			send(client.fd, input.c_str(), input.size(), 0);
			int bytes = recv(client.fd, buff, 1000, 0);
			if (bytes)
				std::cout << GREY "sent" R << std::endl;
			memset(buff, 0, bytes);
		}

		if (input == "exit")
			break;
	}
}

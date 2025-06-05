/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 21:57:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/05 11:59:49 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "classes.hpp"
#include "Server.hpp"
#include <cstring>

#define PURPLE	"\001\033[1;38;2;209;174;231m\002"
#define BLUE	"\001\033[1;38;2;147;222;255m\002"
#define GREY	"\001\033[1;37m\002"
#define RED	"\001\033[1;31m\002"
#define R		"\001\033[1;00m\002"

#define PORT		4242
#define LOCALHOST	"127.0.0.1"
#define SERVER_ADDR	"0.0.0.0"
#define BUFFSIZE	1000
#define MAX_CLIENTS	10
#define TIMEOUT		-1

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
	std::cout << BLUE "Acting as server" R << std::endl;

	Server server(PORT, SERVER_ADDR);
	server.initServer(MAX_CLIENTS);

	while (server.isActive())
	{
		server.readClient(MAX_CLIENTS, TIMEOUT);
		server.treatMsg(BUFFSIZE);
	}
}

void	client()
{
	std::cout << BLUE "Selected: client connection" R << std::endl;

	//instantiate sclient
	Client	client(socket(AF_INET, SOCK_STREAM, 0));
	client.setConnect(AF_INET, LOCALHOST, PORT);

	//Connect to server
	for (int attempt = 0; attempt < 5; attempt++)
	{
		std::cout << GREY "Connection attempt " << attempt + 1 << "/5" R << std::endl;

		if (connect(client.fd, client.gen_addr, sizeof(client.addr)) < 0)
		{
			if (attempt == 4)
				throw (std::runtime_error("Client failed to connect to server"));
			perror("Connect error");
			sleep(1);
		}
		else
			break;
	}
	std::cout << GREY "Successfully connected to server" R << std::endl;

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
	}
}

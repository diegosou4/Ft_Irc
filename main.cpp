/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 21:57:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/12 12:10:15 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "TestClient.hpp"
#include <cstring>
#include <csignal>

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
void	testclient();
void	sigint_handler(int signal);


int main(int ac, char **av)
{
	std::signal(SIGINT, sigint_handler);

	try
	{
		if (ac != 2)
			throw (std::runtime_error("Arg number wrong"));
		if (std::string(av[1]) == "client")
			testclient();
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

void	sigint_handler(int signal)
{
	(void)signal;
	throw (std::runtime_error("\nCtrl-C intercepted - exiting programme"));
}

void	server()
{
	std::cout << BLUE "Acting as server" R << std::endl;

	Server server(PORT, SERVER_ADDR);
	//server.initServer(MAX_CLIENTS);

	//if (server.isActive())
	server.handleClient(MAX_CLIENTS, TIMEOUT);
}

void	testclient()
{
	std::cout << BLUE "Selected: client connection" R << std::endl;

	//instantiate sclient
	TestClient test_client(PORT, LOCALHOST);
	test_client.connectClient();
	test_client.getOnline();
}

void	client()
{
	int client_fd;
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

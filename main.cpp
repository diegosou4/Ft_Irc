/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/26 21:57:31 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/05 13:51:37 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "TestClient.hpp"
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
void	testclient();

int main(int ac, char **av)
{
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

void	testclient()
{
	std::cout << BLUE "Selected: client connection" R << std::endl;

	//instantiate sclient
	TestClient test_client(PORT, LOCALHOST);
	test_client.connectClient();
	test_client.sendMsg(BUFFSIZE);
}

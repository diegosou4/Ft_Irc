#include "Server.hpp"
#include <iostream>
#include "../includes/irc.hpp"
#include <exception>
#include <cstdlib>
#include "Client.hpp"

Server::~Server(){
    std::cout << "Default Destructor" << std::endl;
}

Server::Server(): _opt(1), _server_fd(-1), _host("127.0.0.1"), _port("6667"), _password("")
{
    memset(&_server_adress, 0, sizeof(_server_adress));
}


Server::Server(std::string host, std::string port, std::string password): _opt(1), _server_fd(-1), _host(host), _port(port), _password(password)
{
    memset(&_server_adress, 0, sizeof(_server_adress));
}

void Server::setServer_Adress()
{
    _server_adress.sin_family = AF_INET;
    _server_adress.sin_addr.s_addr = INADDR_ANY; 
    _server_adress.sin_port = htons(static_cast<uint16_t>(atoi(_port.c_str())));
}

void Server::init()
{
    std::cout << "Debug" << std::endl;
    struct pollfd main_fd_server;
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd < 0)
        throw SocketError();

    setServer_Adress();

    int optvalue = 1;
    if (setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &optvalue, sizeof(optvalue)) < 0)
        throw SocketError();
    if (bind(_server_fd, (struct sockaddr *)&_server_adress, sizeof(_server_adress)) < 0)
        throw SocketError();
    if (listen(_server_fd, SOMAXCONN) < 0)
        throw SocketError();


    main_fd_server.fd = _server_fd;
    main_fd_server.events = POLLIN;
    main_fd_server.revents = 0;
    _poll_server.push_back(main_fd_server);
    std::cout << PURPLE "Server is listening on port " << _port << R << std::endl;
}

void Server::Accept()
{
    Client newClient;
    struct sockaddr_in client_addr;
    struct pollfd poll_client;
    socklen_t client_len = sizeof(client_addr);

    newClient.setClientFd(accept(_server_fd,(struct sockaddr *)&client_addr, &client_len));
    if(newClient.getClientFd() < 0)
    {
        // Implement throw
        std::cerr << "Failed to accept connection" << std::endl;
    }
    if (fcntl(newClient.getClientFd(), F_SETFL, O_NONBLOCK) == -1) 
	{
        // Implement throw
        std::cout << "Failed to set no block option" << std::endl;
        return;
    }
    newClient.setIpAddress(inet_ntoa((client_addr.sin_addr)));
    poll_client.fd = newClient.getClientFd();
    poll_client.events = POLLIN;
    poll_client.revents = 0;

    _poll_clients.push_back(newClient);
    _poll_server.push_back(poll_client);
    
    std::cout << "New client connected!" << std::endl;
    
}

void Server::ReceiveData(int current_fd, int current_client)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    Client client = _poll_clients.at(current_client);


    ssize_t bytes_read = read(current_fd, buffer, sizeof(buffer) - 1);

    if(bytes_read <= 0){ 
		std::cout << "Client <" << client.getNickName() << "> Disconnected"  << std::endl;
        close(client.getClientFd());
    }
	else{ 
		buffer[bytes_read] = '\0';
		std::cout << "Client <" << client.getNickName() << "> Data: " << buffer;
	}
}


const char *Server::ParseError::what() const throw()
{
    return ("Error Parse");
}

const char *Server::SocketError::what() const throw()
{
    return("Error socket");
}


int Server::getServerFd() const
{
    return(_server_fd);
}

void Server::run()
{
    std::cout << "Server Running" << std::endl;
    while(true)
    {
        int i = 0;
        while(i < _poll_server.size())
        {
            if(_poll_server[i].revents & POLLIN)
            {
                // Se o fd tiver pronto para ler for o do servidor e pq tem algo chegando, se for outro e um cliente ja conectado
                if(_poll_server[i].fd == _server_fd)
                    Accept();
                else
                    ReceiveData(_poll_server[i].fd , i);
            }

        }

    
    }
}
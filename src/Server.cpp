#include "Server.hpp"
#include <iostream>
#include "../includes/irc.hpp"
#include <exception>
#include <cstdlib>

Server::~Server(){
    std::cout << "Default Destructor" << std::endl;
}

Server::Server()
{


}


Server::Server(char* host, char* port, char* password)
{
    _opt = 1;
    _server_fd = -1;
    _connected_socket = -1;
    _host = host;
    _port = port;
     _password = password;
    // if(_password.length() == 0)
    //     throw ParseError();


}



void Server::init()
{
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(_server_fd < 0)
        throw SocketError();
    _address.sin_family = AF_INET;
	_address.sin_addr.s_addr = inet_addr(_host);
	_address.sin_port = htons(static_cast<uint16_t>(atoi(_port)));
    socklen_t addrlen = sizeof(_address);
	_simple_addr = (struct sockaddr *)&_address;
    setsockopt(_server_fd, SOL_SOCKET, SO_REUSEADDR, &_opt, sizeof(_opt));

    std::cout << "Server running in " << _host << ":" << _port << std::endl;
    if (bind(_server_fd, _simple_addr, addrlen) < 0)
		throw (std::runtime_error("Server binding failed"));

	if (listen(_server_fd, 10) < 0)
		throw (std::runtime_error("Server fails to listen"));

    _connected_socket = accept(_server_fd, _simple_addr, &addrlen);

    if (_connected_socket < 0)
		throw (std::runtime_error("Server failed to accept connection"));
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

sockaddr_in Server::getAdress() const
{
    return(_address);
}

int Server::getConnectedSocket() const
{
    return(_connected_socket);
}
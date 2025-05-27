#include "Server.hpp"
#include <iostream>
#include "../includes/irc.hpp"
#include <exception>
#include <cstdlib>

Server::~Server(){
    std::cout << "Default Destructor" << std::endl;
}

Server::Server()
    : _opt(1), _server_fd(-1), _host("127.0.0.1"), _port("6667"), _password("")
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

    std::cout << PURPLE "Server is listening on port " << _port << R << std::endl;
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
    while (true)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        // Wait for a new client connection
        int client_fd = accept(_server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
        {
            std::cerr << "Failed to accept connection" << std::endl;
            continue;
        }
        std::cout << "New client connected!" << std::endl;


        char buffer[1024];
        write(client_fd, "Please enter the password: ", 26);
        ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        
        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0';
            std::string received_password(buffer);
            received_password.erase(received_password.find_last_not_of("\r\n") + 1); // Remove \n

            if (received_password == _password)
            {
                std::cout << "Client authenticated!" << std::endl;
                const char* welcome = "Welcome to the server!\n";
                write(client_fd, welcome, strlen(welcome));
            }
            else
            {   
                const char* denied = "Wrong password. Connection closed.\n";
                write(client_fd, denied, strlen(denied));
                close(client_fd);
                continue;
            }
        }
        std::cout << "New client connected!" << std::endl;

        // While loop to read data from the client
        while(true)
        {
            ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
            if (bytes_read <= 0)
            {
                std::cerr << "Client disconnected or error reading data" << std::endl;
                break; 
            }
            buffer[bytes_read] = '\0'; 
            std::cout << "Received: " << buffer << std::endl;
        }
   
        close(client_fd); 
    }
}
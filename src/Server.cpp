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
    std::cout << "Accepting new client" << std::endl;
    Client newClient;
    struct sockaddr_in client_addr;
    struct pollfd poll_client;
    socklen_t client_len = sizeof(client_addr);

    int client_fd = accept(_server_fd, (struct sockaddr *)&client_addr, &client_len);
    if (client_fd < 0)
    {
        std::cerr << "Failed to accept connection" << std::endl;
        return;
    }

    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1)
    {
        std::cerr << "Failed to set non-blocking mode" << std::endl;
        close(client_fd);
        return;
    }

    newClient.setClientFd(client_fd);
    newClient.setIpAddress(inet_ntoa(client_addr.sin_addr));

    poll_client.fd = client_fd;
    poll_client.events = POLLIN;
    poll_client.revents = 0;

    // Adiciona o cliente e o poll
    _poll_clients.push_back(newClient);
    _poll_server.push_back(poll_client);

    std::cout << "New client connected!" << std::endl;

    // Envia a primeira mensagem para iniciar o processo de login
    write(client_fd, "Insira seu Nick Name: ", 23);
}


void Server::ReceiveData(int current_fd, int current_client)
{
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    std::cout << "Receiving data from client fd: " << current_fd << std::endl;

    Client* client = NULL;
    for (size_t i = 0; i < _poll_clients.size(); ++i)
    {
        if (_poll_clients[i].getClientFd() == current_fd)
        {
            client = &_poll_clients[i];
            break;
        }
    }

    if (!client)
    {
        std::cerr << "Client not found for fd: " << current_fd << std::endl;
        return;
    }

    _poll_server[current_client].revents = 0;

    ssize_t bytes_read = read(current_fd, buffer, sizeof(buffer) - 1);

    if (bytes_read <= 0)
    {
        std::cout << "Client <" << client->getNickName() << "> Disconnected" << std::endl;
        close(client->getClientFd());
        _poll_server.erase(_poll_server.begin() + current_client);

        for (size_t i = 0; i < _poll_clients.size(); ++i)
        {
            if (_poll_clients[i].getClientFd() == current_fd)
            {
                _poll_clients.erase(_poll_clients.begin() + i);
                break;
            }
        }
        return;
    }

    buffer[bytes_read] = '\0';
    std::string message(buffer);


    std::cout << "Client <" << client->getNickName() << "> Data: " << message << std::endl;

    if (!client->isRegisted())
    {
        switch (client->getRegisterState())
        {
            case WAITING_NICK:
                client->setNickName(message);
                client->setRegisterState(WAITING_USER);
                write(current_fd, "Insira seu User Name: ", 23);
                break;

            case WAITING_USER:
                client->setUserName(message);
                client->setRegisterState(WAITING_PASS);
                write(current_fd, "Insira sua Senha: ", 19);
                break;

            case WAITING_PASS:
                client->SetPassword(message);
                client->setRegisterState(REGISTERED);
                client->setRegisted();
                write(current_fd, "Bem-vindo ao IRC!\n", 19);
                client->WelcomeToIrc(current_fd);
                break;

            default:
                break;
        }
        return; 
    }

    // Cliente já está registrado → processa comandos
    // processCommand(client, message);
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
    std::cout << "Server fd: " << _server_fd << std::endl;
    while (true)
{
    int ret = poll(_poll_server.data(), _poll_server.size(), -1); // Espera indefinidamente
    if (ret < 0)
    {
        std::cerr << "Poll error: " << strerror(errno) << std::endl;
        break;
    }

    for (size_t i = 0; i < _poll_server.size(); ++i)
    {
        if (_poll_server[i].revents & POLLIN)
        {
            if (_poll_server[i].fd == _server_fd)
                Accept();
            else
                ReceiveData(_poll_server[i].fd, i);
        }
    }
}

}
#ifndef SERVER_HPP
#define SERVER_HPP
#include <exception>

#include <string>
#include <netinet/in.h>
#include "../includes/irc.hpp"

class Server{
    private:
        struct sockaddr_in _server_adress;
        struct sockadd   *_simple_server;

        // Lista de Clientes -> Vai guardar fd, nickname, username, realname, etc.
        // Lista de Canais -> vai ter o nome do canal, lista de clientes, etc.


        int _opt;
        std::string _port;
        std::string _host;
        std::string _password;
        int _server_fd;
        void setServer_Adress();
    public:
        ~Server();
        Server();

        Server(std::string host, std::string port,std::string password);
        void init();
        void run();
        class ParseError : public std::exception
        {
            public:
                const char* what() const throw();
        };
        class SocketError : public std::exception
        {
            public:
                const char* what() const throw();
        };
        int getServerFd() const;

};

#endif
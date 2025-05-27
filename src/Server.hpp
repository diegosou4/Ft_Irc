#ifndef SERVER_HPP
#define SERVER_HPP
#include <exception>

#include <string>
#include "../includes/irc.hpp"

class Server{
    private:
        struct sockaddr_in _address;
        struct sockaddr *_simple_addr;
        int _opt;
        char* _port;
        char* _host;
        char* _password;
        int _server_fd;
        int _connected_socket;
    public:
        ~Server();
        Server();
        Server(char* host, char*port,char* password);
        void init();
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
        sockaddr_in getAdress() const;
        int getConnectedSocket() const;
};

#endif
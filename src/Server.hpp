#ifndef SERVER_HPP
#define SERVER_HPP
#include <exception>
#include "Client.hpp"


#include "../includes/irc.hpp"

// struct pollfd {
//     int   fd;         /* file descriptor */
//     short events;     /* requested events */
//     short revents;    /* returned events */
// };

class Client;

class Server{
    private:
        struct sockaddr_in _server_adress;
        struct sockadd   *_simple_server;
        std::vector<pollfd> _poll_server;
        std::vector<Client> _poll_clients;
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
        void Accept();
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
        void RegisterClient(Client& client, int current_fd, const std::string& message);
        void  ReceiveData(int current_fd, int current_client);

        int getCurrentListUser();

        void commandsRotine(Client& client, int current_fd, const std::string& message);
};

#endif
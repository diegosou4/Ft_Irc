#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include "../includes/irc.hpp"



class Client {
    private : 
        int _client_fd;
        std::string _Nick_name;
        std::string _Second_choice;
        std::string _Third_choice;
        std::string _User_name;
        std::string _Ip_address;
        // Lista de canais que o cliente esta vinculado
    public:
        ~Client();
        Client();
        void setClientFd(int fd);
        void setIpAddress(std::string ipaddress);
        void setNickName(std::string nick_name);
        

        std::string getNickName() const;
        int getClientFd() const;
        std::string getIpAddress() const;


};

#endif
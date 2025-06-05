#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include "../includes/irc.hpp"



class Client {
    private : 
        int _client_fd;
        bool _isRegisted;
        bool _haslogout;
        std::string _Ip_address;

        std::string _Nick_name;
        std::string _Second_choice;
        std::string _Third_choice;
        std::string _User_name;
        std::string _Password;
        std::string  _Last_login;

        // Lista de canais que o cliente esta vinculado
    public:
        ~Client();
        Client();

        // Setters
        void setClientFd(int fd);
        void setIpAddress(std::string ipaddress);
        void setNickName(std::string nick_name);
        void setRegisted();
        void SetLastLogin();
        void SetPassword(std::string pass);
    

        // Getters

        std::string getNickName() const;
        int getClientFd() const;
        std::string getIpAddress() const;
        std::string getLastLogin() const;
        std::string getPassword() const;
        bool isRegisted() const;
    
        // Utils Fuctions
        void WelcomeToIrc() const;
        void userInfo() const;


};

#endif
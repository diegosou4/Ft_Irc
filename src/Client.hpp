#ifndef CLIENTE_HPP
#define CLIENTE_HPP

#include "../includes/irc.hpp"

enum RegisterState {
    WAITING_NICK,
    WAITING_SECOND,
    WAITING_THIRD,
    WAITING_USER,
    WAITING_PASS,
    REGISTERED
};

class Client {
    private : 
        int _client_fd;
        RegisterState _registerState;
        bool _haslogout;
        std::string _Ip_address;
        bool _isRegisted;
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
        bool setNickName(std::string nick_name);
        bool setUserName(std::string user_name);
        bool setSecondChoice(std::string second_choice);
        bool setThirdChoice(std::string third_choice);
        void setRegisted();
        void SetLastLogin();
        bool SetPassword(std::string pass);

        void setRegisterState(RegisterState state);
        // Getters

        std::string getNickName() const;
        int getClientFd() const;
        RegisterState getRegisterState() const;
        std::string getIpAddress() const;
        std::string getLastLogin() const;
        std::string getPassword() const;
        bool isRegisted() const;
    
        // Utils Fuctions
        void WelcomeToIrc(int fd) const;
        void userInfo(int fd) const;


};

#endif
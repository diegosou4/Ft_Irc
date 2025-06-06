#include "Client.hpp"
#include <ctime>


Client::~Client(){
    std::cout << "Default Destructor" << std::endl;
}

Client::Client() : _client_fd(-1) , _Nick_name(""), _isRegisted(false), _Second_choice(""), _Third_choice(""), _User_name(""), _Ip_address(""),_haslogout(false), _Last_login(""), _Password(""), _registerState(WAITING_NICK) {
    std::cout << "Default Constuctor" << std::endl;
}

void Client::setRegisted(){
    _isRegisted = true;
}

void Client::setClientFd(int fd){
    _client_fd = fd;
}

void Client::setIpAddress(std::string ipaddress){
    _Ip_address = ipaddress;
}

bool Client::setNickName(std::string nick_name){
    if(nick_name.empty() || nick_name.length() > 20) {
        std::cerr << "Invalid Nickname" << std::endl;
        return false;
    }
    _Nick_name = nick_name;
    return true;
}

bool Client::setUserName(std::string user_name){
    if(user_name.empty() || user_name.length() > 20) {
        std::cerr << "Invalid User Name" << std::endl;
        return false;
    }
    _User_name = user_name;
    return true;
}

bool Client::setSecondChoice(std::string second_choice){
    if(second_choice.empty() || second_choice.length() > 20) {
        std::cerr << "Invalid Second Choice" << std::endl;
        return false;
    }
    _Second_choice = second_choice;
    return true;
}

bool Client::setThirdChoice(std::string third_choice){
    if(third_choice.empty() || third_choice.length() > 20) {
        std::cerr << "Invalid Third Choice" << std::endl;
        return false;
    }
    _Third_choice = third_choice;
    return true;
}

void Client::setRegisterState(RegisterState state){
    _registerState = state;
}

void Client::SetLastLogin()
{
    time_t now = time(0);
    struct tm* tstruct = localtime(&now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tstruct);
    _Last_login = buffer;
}

bool Client::SetPassword(std::string pass){
    if(pass.empty() || pass.length() > 20) {
        std::cerr << "Invalid Password" << std::endl;
        return false;
    }
    _Password = pass;
    return true;
}

int Client::getClientFd() const{
    return _client_fd;
}

std::string Client::getIpAddress() const{
    return _Ip_address;
}

std::string Client::getNickName() const{
    return _Nick_name;
}


std::string Client::getLastLogin() const{
    return _Last_login;
}

std::string Client::getPassword() const {
    return _Password;
}


void Client::WelcomeToIrc(int fd) const{
    write(fd, "\n\n", 2);
    write(fd, "Welcome to Amazing Irc Server\n", 30);
    write(fd, "-------------------------------\n", 32);
    write(fd, _User_name.c_str(), _User_name.length());
    write(fd, "\n", 1);
}


void Client::userInfo(int fd) const{
    write(fd, ("Nick name: " + _Nick_name + "\n").c_str(), 30);
    write(fd, ("User name: " + _User_name + "\n").c_str(), 30);
    write(fd, ("Ip: " + _Ip_address + "\n").c_str(), 30);
}


bool Client::isRegisted() const {
    return _isRegisted;
}

RegisterState Client::getRegisterState() const {
    return _registerState;
}

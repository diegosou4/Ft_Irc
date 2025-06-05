#include "Client.hpp"
#include <ctime>


Client::~Client(){
    std::cout << "Default Destructor" << std::endl;
}

Client::Client() : _client_fd(-1) , _Nick_name(""), _isRegisted(false), _Second_choice(""), _Third_choice(""), _User_name(""), _Ip_address(""),_haslogout(false), _Last_login(""), _Password(""){
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

void Client::setNickName(std::string nick_name){
    _Nick_name = nick_name;
}

void Client::SetLastLogin()
{
    time_t now = time(0);
    struct tm* tstruct = localtime(&now);
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tstruct);
    _Last_login = buffer;
}

void Client::SetPassword(std::string pass){
    _Password = pass;
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


void Client::WelcomeToIrc() const{
    std::cout << "Welcome " << _User_name << "to Amazing Irc Server" << std::endl;
    if(_haslogout == true)
    std::cout << "Your last login" << getLastLogin() << std::endl;
}


void Client::userInfo() const{
    std::cout << "Nick name:" << _Nick_name << std::endl;
    std::cout << "User name:" << _User_name << std::endl;
    std::cout << "Ip:" << _Ip_address << std::endl;
}


bool Client::isRegisted() const {
    return _isRegisted;
}
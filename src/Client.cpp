#include "Client.hpp"


Client::~Client(){
    std::cout << "Default Destructor" << std::endl;
}

Client::Client() : _client_fd(-1) , _Nick_name("As velinhas da Ricksom") {
    std::cout << "Default Constuctor" << std::endl;
}

void Client::setClientFd(int fd)
{
    _client_fd = fd;
}

void Client::setIpAddress(std::string ipaddress){
    _Ip_address = ipaddress;
}

void Client::setNickName(std::string nick_name){
    _Nick_name = nick_name;
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
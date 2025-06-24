/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/05 13:15:41 by cbouvet           #+#    #+#             */
/*   Updated: 2025/06/19 14:34:41 by cbouvet          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// TO BE DONE BY DIEGO

#include "Client.hpp"
#include "Channel.hpp"


Client::Client(): fd(-1), state(OFFLINE)
{

}

Client::Client(int fd): fd(fd), state(OFFLINE)
{

}


Client::Client(Client const &src) : _client_fd(src._client_fd), _real_name(src._real_name), _username(src._username), _Ip_address(src._Ip_address), _state(src._state) 
{
	this->fd = src.fd;
	this->state = src.state;
	this->nickname = src.nickname;
	this->username = src.username;
}

Client::~Client() 
{
	
}



Client &Client::operator=(Client const &src) 
{
	if (this != &src)
	{
		//this->fd = src.fd;
		this->state = src.state;
		this->nickname = src.nickname;
		this->username = src.username;
	}
	return *this;
}

void Client::setClientFd(int fd) 
{
	_client_fd = fd;
}

void Client::setRealname(std::string realname) 
{
	_real_name = realname;
}

void Client::setUsername(std::string username) 
{
	_username = username;
}

void Client::setIpAddress(std::string ip_address) 
{
	_Ip_address = ip_address;
}

void Client::setState(clientState state) 
{
	_state = state;
}



std::string Client::getIpAddress() const 
{
	return _Ip_address;
}

int Client::getClientFd() const 
{
	return _client_fd;
}

std::string Client::getRealname() const 
{
	 return _real_name;
}


clientState Client::getState() const 
{
	return _state;
}

std::string Client::getUsername() const 
{
	return _username;
}

std::string Client::getInChannel() const 
{
	return _in_channel;
}

void Client::setInChannel(std::string in_channel) 
{
	_in_channel = in_channel;
}
std::string Client::getNickname() const 
{
	return _nickname;
}
void Client::setNickname(std::string nickname) 
{
	_nickname = nickname;
}
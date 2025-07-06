/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cbouvet <cbouvet@student.42lisboa.com>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 17:06:16 by cbouvet           #+#    #+#             */
/*   Updated: 2025/07/06 10:14:58 by feden-pe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"
#include "../includes/Channel.hpp"


//----------------- Constructor/Destructor ------------------
Client::Client(): _client_fd(-1), _state(OFFLINE), _username(), _nickname(), _hostname("unknown")
{
	char hostname[1000];

	if (!gethostname(hostname, 1000))
		this->_hostname = hostname;
}

Client::Client(int fd): _client_fd(fd), _state(OFFLINE), _username(), _nickname(), _hostname("unknown")
{
	struct sockaddr_in addr;
	socklen_t addrlen = sizeof(addr);

	if (!getpeername(fd, (struct sockaddr *)&addr, &addrlen))
		this->_hostname = std::string(inet_ntoa(addr.sin_addr));
}

Client::Client(Client const &src) // We might not need - check if should delete
{
	*this = src; // does this use the assignment operator or is it setting this to point to src?
}

Client::~Client()
{}

//------------------ Assignement operator -------------------
Client &Client::operator=(Client const &src)
{
	if (this != &src)
	{
		this->_state = src.getState();
		this->_realname = src.getRealname();
		this->_username = src.getUsername();
		this->_nickname = src.getNickname();
		this->_hostname = src.getHostname();
	}
	return (*this);
}

//------------------------ Setters --------------------------
void	Client::setFd(int fd)
{
	this->_client_fd = fd;
}

void	Client::setState(clientState state)
{
	this->_state = state;
}

void	Client::setRealname(std::string realname)
{
	if (realname[0] == ':')
		realname = &realname[1];

	this->_realname = realname;
}

void	Client::setUsername(std::string username)
{
	this->_username = username;
}

void	Client::setNickname(std::string nickname)
{
	this->_nickname = nickname;
}

void	Client::setHostname(std::string hostname)
{
	this->_hostname = hostname;
}

void	Client::setPrefix()
{
	this->_prefix = ":" + this->_nickname + \
					"!" + this->_username + \
					"@" + this->_hostname;
}

//------------------------ Getters---------------------------
int	Client::getFd() const
{
	return (this->_client_fd);
}

clientState	Client::getState() const
{
	return (this->_state);
}

std::string Client::getRealname() const
{
	return (this->_realname);
}

std::string Client::getUsername() const
{
	return (this->_username);
}

std::string Client::getNickname() const
{
	if (this->_nickname.empty())
		return (this->defaultNickname(this->_client_fd));
	return (this->_nickname);
}

std::string Client::getHostname() const
{
	return (this->_hostname);
}

std::string Client::getPrefix() const
{
	return (this->_prefix);
}

bool	Client::passedNick()const
{
	if (this->_nickname.empty())
		return (false);
	return (true);
}

bool	Client::passedUser()const
{
	if (this->_username.empty() || this->_realname.empty())
		return (false);
	return (true);
}

std::string Client::defaultNickname(int fd) const
{
	std::stringstream ss;
	ss << "ufd_" << fd << "@" << this->_hostname;

	return (ss.str());
}

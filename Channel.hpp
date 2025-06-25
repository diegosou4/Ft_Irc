#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <map>
# include <set>
# include "Client.hpp"
# include "Server.hpp"

class	Channel {
	private :
		std::string						_name;
		std::string						_topic;
		std::string						_password;
		std::vector<Client *>			_members;  // using nick as key
		std::set<std::string>			_operators;  // list of operators
		std::set<std::string>			_invited;  // list of invited clients
		std::set<std::string>			_banned;  // list of banned clients
		bool							_inviteOnly;
		size_t							_limit;
	public :
		Channel(const std::string &name);
		Channel(const std::string &name, const int limit);
		~Channel();

		std::vector<Client *>	getMembers(void) const;

		void		setName(const std::string &name);
		std::string	getName(void) const;

		std::string	setTopic(Client &op, const std::vector<std::string> &msg);
		std::string	getTopic(void) const;

		void		setLimit(const int &limit);

		void		bradcast(const std::string &message, Client *exclude);

		void		setBanned(Client *op, Client *target);
		void		unban(Client *op, Client *target);

		void		setPassword(const std::string &password);
		void		setOperator(Client *op, Client *target);

		bool		isOperator(std::string nick) const;
		bool		isEmpty(void) const;

		void		removeClient(Client *client);
		// JOIN
		std::string		addClient(Client &client, std::vector<std::string> msg);
};

#endif

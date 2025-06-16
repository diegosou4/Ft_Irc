#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <map>
# include <set>

class Client;

class	Channel {
	private :
		std::string				_name;
		std::string				_topic;
		std::string				_password;
		std::map<int, Client *> _clients;  // using client fd as key
		std::set<int>			_operators;  // list of operators
		std::set<int>			_invited;  // list of invited clients
		std::set<int>			_banned;  // list of banned clients
		bool					_inviteOnly;
		size_t					_limit;
	public :
		Channel(const std::string &name);
		Channel(const std::string &name, const int limit);
		~Channel();

		void		setName(const std::string &name);
		std::string	getName(void) const;

		void		setTopic(const std::string &topic);
		std::string	getTopic(void) const;

		void		setLimit(const int &limit);

		void		bradcast(const std::string &message, Client *exclude);

		void		setBanned(Client *op, Client *target);
		void		unban(Client *op, Client *target);

		void		setPassword(const std::string &password);
		void		setOperator(Client *op, Client *target);

		bool		isOperator(int fd) const;
		bool		isEmpty(void) const;

		void		removeClient(Client *client);
		// JOIN
		void		addClient(Client *client, const std::string &password);
};

#endif

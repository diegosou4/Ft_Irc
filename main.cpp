
/* ************************************************************************** */


#include "includes/irc.hpp"
#include "src/Server.hpp"




int main(int ac, char **av)
{
	(void)ac;
		if (std::string(av[1]) == "server")
		{
			Server my_irc = Server(av[2], av[3], av[4]);
			my_irc.init();
			my_irc.run();
		}

	return (0);
}



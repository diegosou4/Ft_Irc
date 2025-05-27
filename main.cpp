
/* ************************************************************************** */


#include "includes/irc.hpp"
#include "src/Server.hpp"


void	client(int &client_fd);
void server(char* host, char* port, char *password);

int main(int ac, char **av)
{
	int server_fd = -1;
	int client_fd = -1;
	try
	{
		if (ac < 2)
			throw (std::runtime_error("Arg number wrong"));
		if (std::string(av[1]) == "client")
			client(client_fd);
		else if (std::string(av[1]) == "server")
			server(av[2],av[3],av[4]);
		else
			throw (std::runtime_error("Wrong arg"));
	}
	catch (std::exception &e)
	{
		if (server_fd > 0)
			close(server_fd);
		if (client_fd > 0)
			close(client_fd);
		std::cerr << RED << e.what() << R << std::endl;
	}

}

void server(char* host, char* port, char *password)
{
	std::cout << GREY "Acting as server" R << std::endl;

	Server my_irc = Server(host,port,password);
	
	my_irc.init();
	

    char buffer[1024];
    ssize_t bytes_read;

    while (true)
    {
        std::string full_message;
        while ((bytes_read = read(my_irc.getConnectedSocket(), buffer, 1024 - 1)) > 0)
        {
            buffer[bytes_read] = '\0';
            full_message += buffer;
            if (bytes_read < 1024 - 1)
                break;
            }   
 
    if (bytes_read <= 0)
    {
        std::cerr << "Cliente desconectado ou erro na leitura." << std::endl;
        break; 
    }

    std::cout << "Mensagem recebida: " << full_message << std::endl;

    
    std::string reply = "Message received";
    send(my_irc.getConnectedSocket(), reply.c_str(), reply.size(), 0);
    }
	
	close(my_irc.getConnectedSocket());
	close(my_irc.getServerFd());
}


void	client(int &client_fd)
{
	std::cout << GREY "Connecting as client" R << std::endl;

	//initiate port
	int port = 4242;

	//instantiate socket
	client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd < 0)
		throw (std::runtime_error("Client socket creation failed"));

	//connect to server
	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = inet_addr("127.0.0.1");
	address.sin_port = htons(port);

	struct sockaddr *simple_addr = (struct sockaddr *)&address;

	//Connect to server
	if (connect(client_fd, simple_addr, sizeof(address)) < 0)
		throw (std::runtime_error("Client failed to connect to server"));

	//take input + send
	char buff[1000];
	while (true)
	{
		std::cout << "> ";
		std::string input;
		getline(std::cin, input);

		if (!input.empty())
		{
			send(client_fd, input.c_str(), input.size(), 0);
			int bytes = recv(client_fd, buff, 1000, 0);
			if (bytes)
				std::cout << GREY "sent" R << std::endl;
			memset(buff, 0, bytes);
		}

		if (input == "exit")
			break;
	}

	//close connection
	close(client_fd);
}
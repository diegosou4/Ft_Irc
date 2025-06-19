#ifndef IRC_HPP
#define IRC_HPP

#include <sys/socket.h>
#include <ostream>
#include <fcntl.h>
#include <string> 
#include <iostream>
#include <sys/types.h> 
#include <iostream>
#include <arpa/inet.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <csignal>
#include <arpa/inet.h>
#include <climits>
#include <unistd.h>
#include <cstring>
#include <poll.h> 
#include <vector> 
#include <unistd.h> 
#include <sys/resource.h>
#include <algorithm>
#include <list>
#include <iostream>


// Pelos limits podevemos ver quantos fd podemos usar e se
// ultrappassar o limite podemos da handle do erro
// struct rlimit {
//            rlim_t  rlim_cur;  /* Soft limit */
//            rlim_t  rlim_max;  /* Hard limit (ceiling for rlim_cur) */
// };


void showIrcServerInfo(int fd, std::string sizeClient);


# define PURPLE	"\001\033[1;38;2;209;174;231m\002"
# define GREY	"\001\033[1;37m\002"
# define RED	"\001\033[1;31m\002"
# define R		"\001\033[1;00m\002"
#endif
#include "unistd.h"
#include <string>

void showIrcServerInfo(int fd, std::string sizeClient)
{
    write(fd, "\n", 1);
    write(fd, "--------------------------------\n", 32);
    write(fd, "IRC Server status!------------------\n", 37);
    write(fd, "--------------------------------", 31);
    write(fd, sizeClient.c_str(), (sizeClient).length());
    write(fd, " Clients Online------------------\n", 33);
}

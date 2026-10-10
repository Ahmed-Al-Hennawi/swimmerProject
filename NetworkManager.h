// This class entirely hides the ugly macOS socket networking logic.
#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <algorithm>

class NetworkManager
{
private:
    int sockfd;

public:
    NetworkManager() : sockfd(-1) {}

    void init(int port)
    {
        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        struct sockaddr_in servaddr;
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(port);
        bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr));
        // Make socket non-blocking
        fcntl(sockfd, F_SETFL, O_NONBLOCK);
    }

    std::string pollCommand()
    {
        if (sockfd < 0)
            return "";
        char buffer[1024];
        int n = recvfrom(sockfd, (char *)buffer, 1024, MSG_DONTWAIT, NULL, NULL);
        if (n > 0)
        {
            buffer[n] = '\0';
            std::string command(buffer);
            // Clean invisible characters
            command.erase(std::remove(command.begin(), command.end(), '\n'), command.end());
            return command;
        }
        return "";
    }

    void cleanup()
    {
        if (sockfd >= 0)
            close(sockfd);
    }
};

#endif
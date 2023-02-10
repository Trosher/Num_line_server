#include "NetProcessing.h"

int net_protocol::NetProcessing::Socket(int domain, int type, int protocol) {
    int result = socket(domain, type, protocol);

    if (result == -1) {
        perror("ERROR: Couldnt get descriptor ");
        exit(EXIT_FAILURE);
    }

    return result;
}

void net_protocol::NetProcessing::Bind(int sockfd, const sockaddr* addr
                                        socklen_t addrlen) {
    if (bind(sockfd, addr, addrlen) == -1) {
        perror("ERROR: Coldnt bind socket ");
        exit(EXIT_FAILURE);
    }
}

sockaddr_in tcp::NetProcessing::InitAddr(int port) {
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    return addr;
}

void net_protocol::NetProcessing::Listen(int socket, int backlog) {
    if (listen(socket, backlog) == -1) {
        perror("ERROR: Coldnt start to listen ");
        exit(EXIT_FAILURE);
    }
}

void net_protocol::NetProcessing::MakeSocketReuseable(int fd) {
    int on = 1;
    if (setsockopt(fd, SQL_socket, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) {
        perror("ERROR: Couldnt make socket reuseable ");
        exit(EXIT_FAILURE);
    }
}
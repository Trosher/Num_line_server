#include "NetProcessing.h"

int net_protocol::NetProcessing::Socket(int domain, int type, int protocol) {
    int result = socket(domain, type, protocol);
    if (result == -1) {
        perror("ERROR: Couldnt get descriptor ");
        exit(EXIT_FAILURE);
    }

    return result;
}

void net_protocol::NetProcessing::Bind(int sockfd, const sockaddr* addr,
                                        socklen_t addrlen) {
    if (bind(sockfd, addr, addrlen) == -1) {
        perror("ERROR: Coldnt bind socket ");
        exit(EXIT_FAILURE);
    }
}

sockaddr_in net_protocol::NetProcessing::IninAddr(int port) {
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
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*)&on, sizeof(on)) < 0) {
        perror("ERROR: Couldnt make socket reuseable ");
        exit(EXIT_FAILURE);
    }
}

void net_protocol::NetProcessing::SetNonBlockingSocket(int fd) {
    int on = 1;
    if (ioctl(fd, FIONBIO, (char*)&on) < 0) {
        perror("ERROR: Couldnt set socket to be non-blocking ");
        exit(EXIT_FAILURE);
    }
}

void net_protocol::NetProcessing::Connect(int socket, const sockaddr* address, 
                                          socklen_t addr_len) {
    if (connect(socket, address, addr_len) == -1) {
        perror("ERROR: Connection failed ");
        exit(EXIT_FAILURE);
    }
}

void net_protocol::NetProcessing::IPConverter(int af, const char* src, void* dst) {
    int result = inet_pton(af, src, dst);
    if (result == 0) {
        printf("inet_pton failed: src does not contain a character"
        " string representing a valid network address in the specified"
        " address family\n");
        exit(EXIT_FAILURE);
    }
    if (result == -1) {
        perror("inet_pton failed ");
        exit(EXIT_FAILURE);
    }
}

std::pair<int, bool> net_protocol::NetProcessing::Write(int fidles, const char* buf,
                                                        size_t nbyte) {
    bool close_connection = false;
    int status = write(fidles, buf, nbyte);
    if (status < 0) {
        if (errno != EWOULDBLOCK) {
            perror("ERROR: error while writing : ");
            close_connection = true;
        }
    } else if (status == 0) {
        std::cout << "Connection close by peer with fd number = " << fidles
              << std::endl;
        close_connection = true;
    }
    return {status, close_connection};
}

std::pair<int, bool> net_protocol::NetProcessing::Read(int fidles, char* buf,
                                                        size_t nbyte) {
    bool close_connection = false;
    int status = read(fidles, buf, nbyte);
    if (status < 0) {
        perror("ERROR: Couldnt read message from FD ");
        close_connection = true;
    }
    return {status, close_connection};
}

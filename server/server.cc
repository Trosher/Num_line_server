#include "server.h"

net_protocol::Server::Server()
            : m_fds_counter_(0),
            m_compress_arr_(false) {
    InitListenPorts();
}

void net_protocol::Server::InitListenPorts() {
    int listening = -1;
    int i = 0;
    listening_ports = SERVER_PORT;
    listening = net_protocol::NetProcessing::Socket(AF_INET, SOCK_STREAM, 0);
    net_protocol::NetProcessing::MakeSocketReuseable(listening);
    net_protocol::NetProcessing::SetNonBlockingSocket(listening);
    addr_size = net_protocol::NetProcessing::IninAddr(listening_ports);
    net_protocol::NetProcessing::Bind(listening, (struct sockaddr*)&addr_size, sizeof(addr_size));
    net_protocol::NetProcessing::Listen(listening, 10);
    m_fds_[0].fd = listening;
    m_fds_[0].events = POLL_IN;
    m_fds_counter_ += 1;
}

net_protocol::Server::~Server() {
    close(m_fds_[0].fd);
}

void net_protocol::Server::EventProcessing(int fd, int &m_fds_counter_) {
    
    m_fds_counter_ -= 1;
}

void net_protocol::Server::CreatingStreamForEventProcessing(int fd) {
    std::thread stream(EventProcessing, fd, std::ref(m_fds_counter_));
    stream.detach();
}

void net_protocol::Server::AddNewUser() {
    int new_fd = 0;
    std::cout << "\nGot the request to add new user\n";
    do {
        if (m_fds_counter_ >= 5) {
            perror("WARING: The maximum number of connections to 
                    the server has been reached ");
            break;
        } 
        new_fd = accept(m_fds_[0].fd, NULL, NULL);
        if (new_fd < 0) {
            if (errno != EWOULDBLOCK) {
                perror("ERROR: Accept failed ");
                m_shutdown_server_ = true;
            }
            break;
        }
        if (new_fd != -1) {
            m_fds_counter_ += 1;
            CreatingStreamForEventProcessing(new_fd);
        }
    } while (new_fd != -1);
}

void net_protocol::Server::CheckingConnectionRequests() {
    int status = 0;
    do {
        // Установка времени ожидания запроса в бесконечный режим
        status = poll(m_fds_, 1, -1);
        std::cout << "poll primal event\n";
        if (status < 0) {
            perror("ERROR: Poll fails while waiting for the request ");
            break;
        } else if (status == 0) {
            perror("ERROR: Poll timed out ");
            break;
        }
        AddNewUser();
    } while (!m_shutdown_server_);
}

void net_protocol::Server::SigHandler(int signum) {
    std::cout << "\nSig exit" << std::endl;
    (void)signum;
    m_shutdown_server_ = true;
}

int main(int argc, char** argv) {
    if (argc == 1) {
        signal(SIGINT, net_protocol::Server::SigHandler);
        net_protocol::Server server;
        server.HandlingCycle();
    } else {
        std::cout << "The server does not accept parameters at startup.\n";
    }
    return 0;
}
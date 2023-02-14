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
    addr_size = net_protocol::NetProcessing::InifAddr(listening_ports);
    net_protocol::NetProcessing::Bind(listening, (struct sockaddr*)&addr_size, sizeof(addr_size));
    net_protocol::NetProcessing::Listen(listening, 10);
    m_fds_[0].fd = listening;
    m_fds_[0].events = POLL_IN;
    m_fds_counter_++;
}

net_protocol::Server::~Server() {
    for (size_t i = 0; i < m_fds_counter_; ++i)
        close(m_fds_[i].fd);
}

void net_protocol::Server::EventProcessing() {
    recv(newSocket, &buf, sizeof(buf), 0);
    
}

void net_protocol::Server::CompressArray() {
    m_compress_arr_ = false;
    for (int i = 0; i < m_fds_counter_ - 1; i++) {
        if (m_fds_[i].fd == -1) {
            for (int j = i; j < m_fds_counter_ - 1; j++) {
                m_fds_[j].fd = m_fds_[j + 1].fd;
            }
            --i;
            --m_fds_counter_;
        }
    }
}

void net_protocol::Server::HandlingCycle() {
    int status = 0;
    do {
        // Установка времени ожидания запроса в бесконечный режим
        status = poll(m_fds_, m_fds_counter_, -1);
        std::cout << "poll primal event\n";
        if (status < 0) {
            perror("ERROR: Poll fails while waiting for the request ");
            break;
        } else if (status == 0) {
            perror("ERROR: Poll timed out ");
            break;
        }
        if (m_compress_arr_) {
            CompressArray();
        }
        EventProcessing();
    } while (!m_shutdown_server_);
}

void net_protocol::Server::SigHandler(int signum) {
    std::cout << "\nSig exit" << std::endl;
    (void)signum;
    m_shutdown_server_ = false;
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
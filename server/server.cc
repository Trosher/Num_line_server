#include "server.h"

net_protocol::Server::Server()
            : m_fds_counter_(0) {
    InitListenPorts();
}

void net_protocol::Server::InitListenPorts() {
    int listening = -1;
    auto listening_ports = SERVER_PORT;
    listening = net_protocol::NetProcessing::Socket(AF_INET, SOCK_STREAM, 0);
    net_protocol::NetProcessing::MakeSocketReuseable(listening);
    net_protocol::NetProcessing::SetNonBlockingSocket(listening);
    auto addr_size = net_protocol::NetProcessing::InintAddr(listening_ports);
    net_protocol::NetProcessing::Bind(listening, (struct sockaddr*)&addr_size, sizeof(addr_size));
    net_protocol::NetProcessing::Listen(listening, 10);
    m_fds_[0].fd = listening;
    m_fds_[0].events = POLL_IN;
    m_fds_counter_ += 1;
}

net_protocol::Server::~Server() {
    mutex_.lock();
    for (int i = 0; i < m_fds_counter_ - 1; i++) {
        close(m_cfds_[0].fd);
    }
    mutex_.unlock();
    close(m_fds_[0].fd);
}

void net_protocol::Server::CompressArray() {
    mutex_.lock();
    for (int i = 0; i < m_fds_counter_ - 1; ++i) {
        if (m_cfds_[i].fd == -1) {
            for (int j = i; j < m_fds_counter_; ++j) {
                m_cfds_[j].fd = m_cfds_[j + 1].fd;
            }
            i -= 1;
            m_fds_counter_ -= 1;
        }
    }
    m_fds_counter_ -= 1;
    mutex_.unlock();
}

void net_protocol::Server::GeneratingNumbersToClient(std::pair<unsigned long int, unsigned long int> seq1, 
                                                    std::pair<unsigned long int, unsigned long int> seq2,
                                                    std::pair<unsigned long int, unsigned long int> seq3,
                                                    int fd) {
    unsigned long long int seq1_buf = seq1.first;
    unsigned long long int seq2_buf = seq2.first;
    unsigned long long int seq3_buf = seq3.first;
    std::pair<int, bool> StateMonitoring;
    StateMonitoring = net_protocol::NetProcessing::Write(fd, "=== START GEN ===\n", 18);
    do {
        std::string Answer;
        if (seq1.first != 0) {
            Answer = std::to_string(seq1_buf) + " ";
        } else {
            Answer =  "  ";
        }
        if (seq2.first != 0) {
            Answer += std::to_string(seq2_buf) + " ";
        } else {
            Answer += "  ";
        }
        if (seq3.first != 0) {
            Answer += std::to_string(seq3_buf);
        }
        Answer += '\n';
        StateMonitoring = net_protocol::NetProcessing::Write(fd, Answer.c_str(), Answer.length());
        seq1_buf += seq1.second;
        seq2_buf += seq2.second;
        seq3_buf += seq3.second;
        if (seq1_buf > ULONG_MAX) {
            seq1_buf = seq1_buf - ULONG_MAX;
        }
        if (seq2_buf > ULONG_MAX) {
            seq2_buf = seq2_buf - ULONG_MAX;
        }
        if (seq3_buf > ULONG_MAX) {
            seq3_buf = seq3_buf - ULONG_MAX;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } while (!StateMonitoring.second && !m_shutdown_server_);
}

void net_protocol::Server::RequestProcessing(int fd) {
    std::pair<unsigned long int, unsigned long int> seq1(0, 0); 
    std::pair<unsigned long int, unsigned long int> seq2(0, 0);
    std::pair<unsigned long int, unsigned long int> seq3(0, 0);
    std::pair<int, bool> StateMonitoring;
    StateMonitoring = net_protocol::NetProcessing::Write(fd, HELLO_CLIENT, strlen(HELLO_CLIENT));
    do {
        char buf[BUFF_SIZE]{};
        StateMonitoring = net_protocol::NetProcessing::Read(fd, buf, BUFF_SIZE);
        try {
            if (StateMonitoring.first > 0) {
                if (net_protocol::RequsetHandler::CheckingValidityParamRequest(buf) < 0) {
                        StateMonitoring = net_protocol::NetProcessing::Write(fd, INCORECT_REQUEST, strlen(INCORECT_REQUEST));
                } else {
                    Requests RequestIdentifier = net_protocol::RequsetHandler::DefinitionRequest(buf);
                    if (RequestIdentifier != EXPORTSEQ && RequestIdentifier != ERROR_R) {
                        if (RequestIdentifier == SEQ1) {
                            seq1 = net_protocol::RequsetHandler::GetParamRequest(buf);
                        } else if (RequestIdentifier == SEQ2) {
                            seq2 = net_protocol::RequsetHandler::GetParamRequest(buf);
                        } else if (RequestIdentifier == SEQ3) {
                            seq3 = net_protocol::RequsetHandler::GetParamRequest(buf);
                        }
                    } else if (RequestIdentifier == EXPORTSEQ) {
                        if (net_protocol::RequsetHandler::CheckingValidityParamSaved(seq1, seq2, seq3) < 0) {
                            StateMonitoring = net_protocol::NetProcessing::Write(fd, INCORECT_REQUEST_NON_SEQ, strlen(INCORECT_REQUEST_NON_SEQ));
                        } else {
                            GeneratingNumbersToClient(seq1, seq2, seq3, fd);
                            break;
                        }
                    } else if (RequestIdentifier == ERROR_R) {
                        StateMonitoring = net_protocol::NetProcessing::Write(fd, INCORECT_REQUEST, strlen(INCORECT_REQUEST));
                    }
                }             
            }
        } catch (const std::runtime_error& err) {
            StateMonitoring = net_protocol::NetProcessing::Write(fd, ERROR_REG, strlen(ERROR_REG));
            break;
        }
    } while (!StateMonitoring.second && !m_shutdown_server_);
    close(fd);
    CompressArray();
}

void net_protocol::Server::CreatingStreamForRequestProcessing(int fd) {
    std::thread stream([&]()
    {
        net_protocol::Server::RequestProcessing(fd);
    });
    stream.detach();
}

void net_protocol::Server::AddNewUser() {
    int new_fd = 0;
    std::cout << "\nGot the request to add new user\n";
    do {
        if (m_fds_counter_ > 5) {
            perror("WARING: The maximum number of connections to the server has been reached \n");
            break;
        } else {
            new_fd = accept(m_fds_[0].fd, NULL, NULL);
        }
        if (new_fd < 0) {
            if (errno != EWOULDBLOCK) {
                perror("ERROR: Accept failed \n");
                m_shutdown_server_ = true;
            }
            break;
        }
        if (new_fd != -1) {
            m_cfds_[m_fds_counter_ - 1].fd = new_fd;
            m_fds_counter_ += 1;
            CreatingStreamForRequestProcessing(new_fd);
        }
    } while (new_fd != -1);
}

void net_protocol::Server::CheckingConnectionRequests() {
    int status = 0;
    do {
        std::cout << "poll primal event\n";
        status = poll(m_fds_, 1, -1);
        if (status < 0) {
            perror("ERROR: Poll fails while waiting for the request \n");
            break;
        } else if (status == 0) {
            perror("ERROR: Poll timed out \n");
            break;
        }
        AddNewUser();
    } while (!m_shutdown_server_);
}

void net_protocol::Server::SigHandler(int /*signum*/) {
    std::cout << "\nSig exit" << std::endl;
    m_shutdown_server_ = true; 
}

int main() {
    std::cout << "If you want to stop the server use this: Ctrt+C or Ctrl+Z" << std::endl;
    signal(SIGINT, net_protocol::Server::SigHandler);
    signal(SIGTSTP, net_protocol::Server::SigHandler);
    net_protocol::Server server;
    server.CheckingConnectionRequests();
    return 0;
}

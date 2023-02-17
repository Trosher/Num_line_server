#include "server.h"

net_protocol::Server::Server()
            : m_fds_counter_(0) {
    InitListenPorts();
}

void net_protocol::Server::InitListenPorts() {
    int listening = -1;
    int i = 0;
    auto listening_ports = SERVER_PORT;
    listening = net_protocol::NetProcessing::Socket(AF_INET, SOCK_STREAM, 0);
    net_protocol::NetProcessing::MakeSocketReuseable(listening);
    net_protocol::NetProcessing::SetNonBlockingSocket(listening);
    auto addr_size = net_protocol::NetProcessing::IninAddr(listening_ports);
    net_protocol::NetProcessing::Bind(listening, (struct sockaddr*)&addr_size, sizeof(addr_size));
    net_protocol::NetProcessing::Listen(listening, 10);
    m_fds_[0].fd = listening;
    m_fds_[0].events = POLL_IN;
    m_fds_counter_ += 1;
}

net_protocol::Server::~Server() {
    close(m_fds_[0].fd);
}

void net_protocol::Server::GeneratingNumbersToClient(std::pair<unsigned long int, unsigned long int> seq1, 
                                                    std::pair<unsigned long int, unsigned long int> seq2,
                                                    std::pair<unsigned long int, unsigned long int> seq3,
                                                    int fd) {
    unsigned long long int seq1_buf = seq1.first;
    unsigned long long int seq2_buf = seq2.first; 
    unsigned long long int seq3_buf = seq3.first;
    std::pair<int, bool> StateMonitoring;
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
            Answer += std::to_string(seq3_buf) + '\n';
        }
        StateMonitoring = net_protocol::NetProcessing::Write(fd, Answer.c_str(), Answer.length());
        seq1_buf += seq1.second;
        seq2_buf += seq2.second;
        seq3_buf += seq3.second;
        if (seq1_buf > ULONG_MAX) {
            seq1_buf -= ULONG_MAX;
        }
        if (seq2_buf > ULONG_MAX) {
            seq2_buf -= ULONG_MAX;
        }
        if (seq3_buf > ULONG_MAX) {
            seq3_buf -= ULONG_MAX;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    } while (!StateMonitoring.second && !m_shutdown_server_);
}

void net_protocol::Server::RequestProcessing(int fd, int &m_fds_counter_) {
    std::pair<unsigned long int, unsigned long int> seq1(0, 0); 
    std::pair<unsigned long int, unsigned long int> seq2(0, 0);
    std::pair<unsigned long int, unsigned long int> seq3(0, 0);
    std::pair<int, bool> StateMonitoring;
    char buf[BUFF_SIZE]{};
    do {
        StateMonitoring = net_protocol::NetProcessing::Read(fd, buf, BUFF_SIZE);
        if (StateMonitoring.first > 0) {
            Requests RequestIdentifier = net_protocol::RequsetHandler::DefinitionRequest(buf);
            if (RequestIdentifier != EXPORTSEQ || RequestIdentifier != ERROR_R) {
                if (net_protocol::RequsetHandler::CheckingValidityParamRequest(buf) < 0) {
                    char Answer[] = "WARNING: The request was made incorrectly.\nUse parameters with a lower value than ULONG_MAX ";
                    StateMonitoring = net_protocol::NetProcessing::Write(fd, Answer, strlen(Answer));
                } else {
                    if (RequestIdentifier == SEQ1) {
                        seq1 = net_protocol::RequsetHandler::GetParamRequest(buf);
                    } else if (RequestIdentifier == SEQ2) {
                        seq2 = net_protocol::RequsetHandler::GetParamRequest(buf);
                    } else if (RequestIdentifier == SEQ3) {
                        seq3 = net_protocol::RequsetHandler::GetParamRequest(buf);
                    }
                }
            } 
            if (RequestIdentifier == EXPORTSEQ) {
                if (net_protocol::RequsetHandler::CheckingValidityParamSaved(seq1, seq2, seq3) < 0) {
                    char Answer[] = "WARNING: The request was made incorrectly.\n More than one seq has not been set with the correct parameters,\nnumber generation is impossible ";
                    StateMonitoring = net_protocol::NetProcessing::Write(fd, Answer, strlen(Answer));
                } else {
                    GeneratingNumbersToClient(seq1, seq2, seq3, fd);
                    break;
                }
            }
            if (RequestIdentifier == ERROR_R) {
                char Answer[] = "WARNING: The request was made incorrectly ";
                StateMonitoring = net_protocol::NetProcessing::Write(fd, Answer, strlen(Answer));
            }
        }
    } while (!StateMonitoring.second && !m_shutdown_server_);
    close(fd);
    m_fds_counter_ -= 1;
}

void net_protocol::Server::CreatingStreamForRequestProcessing(int fd) {
    std::thread stream([&]()
    {
        net_protocol::Server::RequestProcessing(fd, std::ref(m_fds_counter_));
    });
    stream.detach();
}

void net_protocol::Server::AddNewUser() {
    int new_fd = 0;
    std::cout << "\nGot the request to add new user\n";
    do {
        if (m_fds_counter_ >= 5) {
            perror("WARING: The maximum number of connections to the server has been reached ");
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
            CreatingStreamForRequestProcessing(new_fd);
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

int main() {
    signal(SIGINT, net_protocol::Server::SigHandler);
    net_protocol::Server server;
    server.CheckingConnectionRequests();
    return 0;
}
#include "client.h"

Client::~Client() {
    regfree(&m_seqRegex);
    regfree(&m_exportSeqRegex);
}

void Client::compileRegexes() {
    char errBuff[BUFF_SIZE]{};

    int regStatus = regcomp(&m_seqRegex, kSEQREGXPATTERN.c_str(), REG_EXTENDED);
    if (regStatus != 0) {
        (void)regerror(regStatus, &m_seqRegex, errBuff, BUFF_SIZE);  // ignoring returned value
        throw std::runtime_error("Error trying to compile seqRegex: " + std::string(errBuff));
    }

    regStatus = regcomp(&m_exportSeqRegex, kEXPORTREGXPATTERN.c_str(), REG_EXTENDED);
    if (regStatus != 0) {
        (void)regerror(regStatus, &m_exportSeqRegex, errBuff, BUFF_SIZE);
        throw std::runtime_error("Error trying to compile exportSeqRegex: " + std::string(errBuff));
    }
}

int Client::init(const std::string& ip, int port) {
    int status = 0;
    net_protocol::NetProcessing::IPConverter(AF_INET, ip.c_str(), &m_address);
    m_address.sin_port = htons(port);
    m_address.sin_family = AF_INET;
    m_sockFd = net_protocol::NetProcessing::Socket(AF_INET, SOCK_STREAM, 0);
    net_protocol::NetProcessing::Connect(m_sockFd, (const sockaddr*)&m_address, sizeof(m_address));
    try {
        compileRegexes();
    } catch (const std::runtime_error& err) {
        std::cout << err.what();
        status = -1;
    }
    return status;
}

void Client::processing() {
    std::string requestBuff;
    std::pair<int, bool> StateMonitoring;
    do {
        getline(std::cin, requestBuff);
        if (regexec(&m_seqRegex, requestBuff.c_str(), 0, nullptr, 0) == 0) {
            StateMonitoring = net_protocol::NetProcessing::Write(m_sockFd, requestBuff.c_str(), requestBuff.size());
        } else if (regexec(&m_exportSeqRegex, requestBuff.c_str(), 0, nullptr, 0) == 0) {
            StateMonitoring = net_protocol::NetProcessing::Write(m_sockFd, requestBuff.c_str(), requestBuff.size());
                while (!StateMonitoring.second) {
                    char responseBuff[BUFF_SIZE]{};
                    StateMonitoring = net_protocol::NetProcessing::Read(m_sockFd, responseBuff, BUFF_SIZE);
                    std::cout << responseBuff << std::endl;
                    if (responseBuff[0] == 'W') {
                        break;
                    }
                }
        } else {
            std::cout << "Error: Incorrect command!" << std::endl;
        }
    } while (!StateMonitoring.second);
}

std::string Client::getRequest() {
    return std::string(
        m_request[0] +
        m_request[1] +
        m_request[2]
    );
}

size_t Client::getRequestMessageLength() {
    size_t len = 0;
    for (const auto& it : m_request) {
        len += it.length();
    }
    return len;
}

int main(int argc, char** argv) {
    Client client;
    client.init("127.0.0.1", 85);
    client.processing();
    return 0;
}
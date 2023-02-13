#include "client.h"

Client::~Client() {
    regfree(&m_seqRegex);
    regfree(&m_exportSeqRegex);
}

void Client::compileRegexes() {
    char errBuff[BUFF_LEN]{};

    int regStatus = regcomp(&m_seqRegex, kSEQREGXPATTERN.c_str(), REG_EXTENDED);
    if (regStatus != 0) {
        (void)regerror(regStatus, &m_seqRegex, errBuff, BUFF_LEN);  // ignoring returned value
        throw std::runtime_error("Error trying to compile seqRegex: " + std::string(errBuff));
    }

    regStatus = regcomp(&m_exportSeqRegex, kEXPORTREGXPATTERN.c_str(), REG_EXTENDED);
    if (regStatus != 0) {
        (void)regerror(regStatus, &m_exportSeqRegex, errBuff, BUFF_LEN);
        throw std::runtime_error("Error trying to compile exportSeqRegex: " + std::string(errBuff));
    }
}

int Client::init(const std::string& ip, int port) {
    int status = 0;
    tcp::NetHandler::IPConverter(AF_INET, ip.c_str(), &m_address);
    m_address.sin_port = htons(port);
    m_address.sin_family = AF_INET;
    m_sockFd = tcp::NetHandler::Socket(AF_INET, SOCK_STREAM, 0);
    tcp::NetHandler::Connect(m_sockFd, (const sockaddr*)&m_address, sizeof(m_address));
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
    char responseBuff[BUFF_LEN];
    while (true) {
        getline(std::cin, requestBuff);
        if (regexec(&m_seqRegex, requestBuff.c_str(), 0, nullptr, 0) == 0) {
            m_request.push_back(std::move(requestBuff));
        } else if (regexec(&m_exportSeqRegex, requestBuff.c_str(), 0, nullptr, 0) == 0) {
            if (m_request.size() == 3) {
                tcp::NetHandler::Write(m_sockFd, getRequest().c_str(), getRequestMessageLength());
                while (true) {
                    read(m_sockFd, &responseBuff, BUFF_LEN);
                    // можно добавить обработку, что если пришел пустой пакет, делаем дисконнект
                    // (read возвращает число вычитанных из фдшника байт)
                    std::cout << responseBuff << std::endl;
                }
            } else {
                std::cout << "Error: Incorrect request!" << std::endl;
                exit(EXIT_FAILURE);
            }
        } else {
            std::cout << "Error: Incorrect command!" << std::endl;
        }
    }
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

int main() {
    Client client;
    client.init("127.0.0.1", 80);
    client.processing();
    return 0;
}
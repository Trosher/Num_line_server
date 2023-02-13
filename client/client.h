#ifndef SRC_CLIENT_CLIENT_H_
#define SRC_CLIENT_CLIENT_H_

#include <string>
#include <iostream>
#include "../NetProcessing/NetProcessing.h" // Оебртка стандартных методов
#include <regex.h>
#include <vector>
#include <exception>

const static std::string kSEQREGXPATTERN = "^(seq)([1-3]) ([0-9]+) ([0-9]+)$";  // не уверен нужно ли экранировать пробелы
const static std::string kEXPORTREGXPATTERN = "^(export seq)$";


class Client {
    public:
        Client() = default;
        ~Client();
        int init(const std::string& ip, int port);
        void processing();

    private:
        sockaddr_in m_address{};
        int m_sockFd;
        regex_t m_seqRegex;
        regex_t m_exportSeqRegex;
        std::vector<std::string> m_request;

        void compileRegexes();
        std::string getRequest();
        size_t getRequestMessageLength();

};

#endif // SRC_CLIENT_CLIENT_H_
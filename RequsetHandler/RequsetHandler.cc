#include "RequsetHandler.h"

void net_protocol::RequsetHandler::compileRegexes(regex_t& m_seqRegex, regex_t& m_exportSeqRegex) {
    char errBuff[255]{};
    const std::string kSEQREGXPATTERN = "^(seq)([1-3]) ([0-9]+) ([0-9]+)$";
    const std::string kEXPORTREGXPATTERN = "^(export seq)$"; 
    int regStatus = regcomp(&m_seqRegex, kSEQREGXPATTERN.c_str(), REG_EXTENDED);
    if (regStatus != 0) {
        (void)regerror(regStatus, &m_seqRegex, errBuff, 255);
        throw std::runtime_error("Error trying to compile seqRegex: " + std::string(errBuff));
    }

    regStatus = regcomp(&m_exportSeqRegex, kEXPORTREGXPATTERN.c_str(), REG_EXTENDED);
    if (regStatus != 0) {
        (void)regerror(regStatus, &m_exportSeqRegex, errBuff, 255);
        throw std::runtime_error("Error trying to compile exportSeqRegex: " + std::string(errBuff));
    }
}

Requests net_protocol::RequsetHandler::DefinitionRequest(const char *buf) {
    Requests res;
    if (buf[3] == '1') {
        res = SEQ1;
    } else if (buf[3] == '2') {
        res = SEQ2;
    } else if (buf[3] == '3') {
        res = SEQ3;
    } else if (buf[3] == 'o') {
        res = EXPORTSEQ;
    } else {
        res = ERROR_R;
    }
    return res;
}

std::pair<unsigned long int, unsigned long int> net_protocol::RequsetHandler::GetParamRequest(const char *buf) {
    unsigned long int FirstParam, SecondParam;
    int i = 5;
    FirstParam = CharStrToNum(buf + i);
    while (buf[i] >= '0' && buf[i] <= '9') {
        i += 1;
    }
    i += 1;
    SecondParam = CharStrToNum(buf + i);
    if (FirstParam == 0 || SecondParam == 0) {
        FirstParam = 0;
        SecondParam = 0;
    }
    return {FirstParam, SecondParam};
}

int net_protocol::RequsetHandler::CheckingValidityParamRequest(const char *buf) {
    int CorrecktParamFlag = 0;
    regex_t m_seqRegex, m_exportSeqRegex;
    compileRegexes(m_seqRegex, m_exportSeqRegex);
    if (regexec(&m_seqRegex, buf, 0, nullptr, 0) != 0) {
        std::pair<unsigned long long int, unsigned long long int> CheckingParam(GetParamRequest(buf));
        if (CheckingParam.first > ULONG_MAX || CheckingParam.second > ULONG_MAX) {
            CorrecktParamFlag = -1;
        }
    } else if (regexec(&m_exportSeqRegex, buf, 0, nullptr, 0) != 0) {
        ;
    } else {
        CorrecktParamFlag = -1;
    }
    regfree(&m_seqRegex);
    regfree(&m_exportSeqRegex);
    return CorrecktParamFlag;
}

int net_protocol::RequsetHandler::CheckingValidityParamSaved(std::pair<unsigned long int, unsigned long int> seq1, 
                                                            std::pair<unsigned long int, unsigned long int> seq2,
                                                            std::pair<unsigned long int, unsigned long int> seq3) {
    int res = 0;
    if (seq1.first == 0 && seq2.first == 0 && seq3.first == 0) {
        res = -1;
    }
    return res;
}

unsigned long int net_protocol::RequsetHandler::CharStrToNum(const char *buf) {
    int i = 0;
    unsigned long int res = 0;
    while (buf[i] >= '0' && buf[i] <= '9') {
        res = res * 10 + (buf[i] - '0');
        i += 1;
    }
    return res;
}
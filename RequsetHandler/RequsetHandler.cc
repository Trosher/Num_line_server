#include "RequsetHandler.h"

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
    std::pair<unsigned long long int, unsigned long long int> CheckingParam(GetParamRequest(buf));
    if (CheckingParam.first > ULONG_MAX || CheckingParam.second > ULONG_MAX) {
        CorrecktParamFlag = -1;
    }
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
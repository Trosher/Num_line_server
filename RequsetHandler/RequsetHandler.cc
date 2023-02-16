#include "RequsetHandler.h"

Requests net_protocol::RequsetHandler::DefinitionRequest(char &buf) {
    Request res;
    if (buf[3] == '1') {
        res = SEQ1;
    } else if (buf[3] == '2') {
        res = SEQ2;
    } else if (buf[3] == '3') {
        res = SEQ3;
    } else if (buf[3] == 'o') {
        res = EXPORTSEQ
    } else {
        res = ERROR_R;
    }
    return res;
}

std::pair<unsigned long int, unsigned long int> net_protocol::RequsetHandler::GetParamRequest(char &buf) {
    unsigned long int FirstParam, SecondParam;
    int i = 6;
    FirstParam = atoi(buf + i);
    while (buf[i] >= '0' && buf[i] <= '9') {
        i += 1;
    }
    i += 1;
    SecondParam = atoi(buf + i);
    if (FirstParam == 0 || SecondParam == 0) {
        FirstParam = 0;
        SecondParam = 0;
    }
    return {FirstParam, SecondParam};
}

int net_protocol::RequsetHandler::CheckingValidityParamRequest(char &buf) {
    int CorrecktParamFlag = 0;
    std::pair<unsigned long long int, unsigned long long int> CheckingParam(GetParamRequest);
    if (CheckingParam.first > ULONG_MAX || CheckingParam.second > ULONG_MAX) {
        CorrecktParamFlag = -1;
    }
    return CorrecktParamFlag;
}

int net_protocol::RequsetHandler::CheckingValidityParamSaved(std::pair<unsigned long int, unsigned long int> &seq) {
    int res = 0;
    if (seq[0].first == 0 && seq[1].first == 0 && seq[2].first == 0) {
        res = -1;
    }
    return res;
}
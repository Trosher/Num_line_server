#ifndef RQUESTPARSER_RQUESTHANDLER_H_
#define RQUESTPARSER_RQUESTHANDLER_H_

#include <stdio.h> // Либа операция стандартного ввода и вывода
#include <stdlib.h> // Либа для работы с памятью, преобразованием типов и т.п.
#include <string.h> // Либа для работы с строками
#include <iostream> // Функции для работы с вводом-выводом
#include <regex.h> // Работа с регулярными запросами
#include <limits.h> // Заголовки максимальных значений типов

enum Requests {
  SEQ1,
  SEQ2,
  SEQ3,
  EXPORTSEQ,
  ERROR_R
};

namespace net_protocol {
    class RequsetHandler {
        public:
            /*
                * Определение того какой запрос нам пришёл
                
                Incoming:
                    buf - Буфер хранящий запрос

                Return:
                    * Если запрос не удалось определить возращает ERROR_R 
            */
            static Requests DefinitionRequest(char &buf);

            /*
                * Забирает параметры запросов seq1, seq2 , seq3
                
                Incoming:
                    buf - Буфер хранящий запрос

                Return:
                    * Пара параметорв из запроса
            */
            static std::pair<int, int> GetParamRequest(char &buf);

            /*
                * Проверяет валидность параметров полученого запроса
                
                Incoming:
                    buf - Буфер хранящий запрос

                Return:
                    * Если параметры запроса были невалидны возращает -1, иначе 0
            */
            static int CheckingValidityParamRequest(char &buf);

            /*
                * Проверяет возможность отправки ответа клиенту 
                по сохранённым параметрам
                
                Incoming:
                    seq - Масив сохранёных параметров

                Return:
                    * Если параметры запроса были невалидны возращает -1, иначе 0
            */
            static int CheckingValidityParamSaved(std::pair<unsigned long int, unsigned long int> &seq);

    }; // class RequsetHandler
} // namespace net_protocol

#endif // RQUESTPARSER_RQUESTHANDLER_H_
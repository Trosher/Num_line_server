#ifndef NETPROCESSING_NETPROCESSING_H_
#define NETPROCESSING_NETPROCESSING_H_

#include <arpa/inet.h> // Функции для работы с числовыми ip
#include <errno.h> // Обявленые макросы для индефикации ошибки через их код
#include <fcntl.h> // Либа для работы с файловыми дискрипторами
#include <netinet/in.h> // Либа для работы с IP-адресами, а также номерами портов TCP и UDP
#include <stdio.h> // Либа операция стандартного ввода и вывода
#include <stdlib.h> // Либа для работы с памятью, преобразованием типов и т.п.
#include <string.h> // Либа для работы с строками
#include <sys/ioctl.h> // Средство управления аппаратными утросйствами через их дискриптор
#include <sys/poll.h> // Ловля событий по дискриторам о возможноти ввода-вывода
#include <sys/socket.h> // Базовые функции сокетов BSD и структур данных
#include <sys/time.h> // Таймер для процессов 
#include <sys/types.h> // Определения некоторых типов
#include <unistd.h> // Доступ к API POSIX/Unix системы
#include <iostream> // Функции для работы с вводом-выводом

constexpr size_t BUFF_SIZE = 1024;

namespace net_protocol {
    class NetProcessing {
        public:
            /*
                * Создает конечную точку для связи и 
                возвращает дескриптор сокета
                
                Incoming:
                    domain - Домен/адресса связи в пределах 
                            которого будет осуществлятся связь

                    type - Тип создаваемого доменна

                    protocol - Запрошеный протокол

                Return:
                    * Если результат работы socket() не ошибка
                    возращает результат вызова
            */
            static int Socket(int domain, int type, int protocol);

            /*
                * Привязывает уникальное локальное имя 
                к сокету с дескриптором socket

                Incoming:
                    sockfd - Дескриптор сокета, возвращенный 
                            предыдущим вызовом socket()

                    addr - Указатель на структуру sockaddr, содержащую имя, 
                        которое должно быть привязано к сокету

                    addrlen - Размер адреса в байтах
            */
            static void Bind(int sockfd, const sockaddr* addr, socklen_t addrlen);

            /*
                * Инициализирует структуру sockaddr_in

                Incoming:
                    port - Порт, который должен быть подключен

                Return:
                    возвращает структуру sockaddr_in, заполненную новыми данными

            */
            static sockaddr_in IninAddr(int port);

            /*
                * Указывает на готовность принимать клиентские запросы на подключение 
                и создает очередь запросов на подключение длиной backlog для постановки 
                в очередь входящих запросов на подключение

                Incoming:
                    socket - Дескриптор сокета

                    backlog - Определяет максимальную длину 
                            очереди ожидающих подключений
            */
            static void Listen(int socket, int backlog);

            /*
                * С помощью установки параметров связаных с сокетом делает
                сокет пригодным для повторного использования

                Incoming:
                    fd - Дескриптор сокета
            */
            static void MakeSocketReuseable(int fd);

            /*
                * Делает сокет не блокирующим

                Incoming:
                    fd - Дескриптор сокета
            */
            static void SetNonBlockingSocket(int fd);

            /*
                * Пытается установить соединение между двумя сокетами

                Incoming:
                    socket - Дескриптор сокета

                    address - Указатель на структуру адреса сокета, 
                            к которому будет предпринята попытка подключения

                    addr_len - Размер адреса сокета, на который указывает адрес в байтах

            */
            static void Connect(int socket, const sockaddr* address, socklen_t addr_len);

            /*
                * Пытается установить соединение между двумя сокетами

                Incoming:
                    af - Семейство адресов

                    src - Строка, заверщающаяся нулем. Он указывает на передаваемую строку

                    dst - Указывает на буфер, в который inet_pton() сохраняет числовой адрес

            */
            static void IPConverter(int af, const char* src, void* dst);

            /*
                * Записывает N байт из buf в файл или сокет, связанный с fs. 
                N не должно быть больше INT_MAX

                Incoming:
                    fidles - Дескриптор файла или сокета

                    buf - Указатель на буфер, содержащий данные, подлежащие записи

                    ndyte - Длина в байтах буфера, на который указывает параметр buf

                Return:
                    возвращает пару из статуса выполнения write() и флага на закрытия соиденения

            */
            static std::pair<int, bool> Write(int fidles, const char* buf, size_t ndyte);

            /*
                * Из файла, указанного файловым дескриптором fidles, функция read() 
                считывает N байт входных данных в область памяти, указанную buf

                Incoming:
                    fidles - Дескриптор файла или сокета

                    buf - Указатель на буфер, содержащий данные, подлежащие записи

                    ndyte - Длина в байтах буфера, на который указывает параметр buf

                Return:
                    возвращает пару из статуса выполнения read() и флага на закрытия соиденения
            */
            static std::pair<int, bool> Read(int fidles, char* buf, size_t ndyte);
    }; // class NetProcessing
} // namespace net_protocol

#endif // NETPROCESSING_NETPROCESSING_H_
#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include <algorithm> //набор функций для выполнения алгоритмических операций 
                    //над контейнерами и над другими последовательностями
#include <vector> // Реализация динамического массива
#include <csignal> // Макросы сигналов и функции их отработки
#include <fstream> // набор классов, методов и функций, которые предоставляют 
                  // интерфейс для чтения/записи данных из/в файл
#include <queue> // Реализация очереди
#include <thread> // Библиотека для работы с потоками

#include "../NetProcessing/NetProcessing.h" // Оебртка стандартных методов для работы с сетью
#include "../RequsetHandler/RequsetHandler.h" // Обработка запросов клиента

bool m_shutdown_server_ = false;
constexpr int SERVER_PORT = 85;

namespace net_protocol {
    class Server {
        public:
            /*
                * Конструктор класса сервер

                * Создает fd для прослушивающего сокета
                
                * Настраивает этот сокет на прослушивание с очередью из 10 клиентов
                
                * Создаёт порт, который сервер будет использовать 
                в качестве прослушивающего для подключения нового клиента
            */
            Server();

            // Диструктор класса сервр
            ~Server();

            /*
                * Функция обработки сигнала Ctrl + C

                Incoming:
                    signum - входной сигнал
            */ 
            static void SigHandler(int signum);
            
            /*
                * Функция проверки наличия новых подключений

                * Основной метод, вызывающий poll()
            */ 
            void CheckingConnectionRequests();

        private:

            /*
                * Обрабатывает события конкретного клиента

                * Вызывает Disconnect User(), 
                когда получает запрос на отключение.

                Incoming:
                    fd - дискриптор клиента
            */
            void RequestProcessing(int fd, int &m_fds_counter_);

            /*
                * Генерирует числа для клиента и отправляет их

                Incoming:
                    seq* - Праметры команд клиента

                    fd - Дискриптор клиента
            */
            void GeneratingNumbersToClient(std::pair<unsigned long int, unsigned long int> seq1, 
                                            std::pair<unsigned long int, unsigned long int> seq2,
                                            std::pair<unsigned long int, unsigned long int> seq3,
                                            int fd);

            // Создание потока под RequestProcessing
            void CreatingStreamForRequestProcessing(int fd);

            // Инициализация и настройка порта для подключения
            void InitListenPorts();
            
            // Функция добавления нового юзера
            void AddNewUser();

            // Буфер клиента
            char buf[BUFF_SIZE];
            // Список отслеживаемых файловых дискрипторов
            pollfd m_fds_[1]{};
            // Счетчик количества отслеживаемых файловых дискрипторов
            int m_fds_counter_;

    }; // class Server
} // namespace net_protocol

#endif // SERVER_SERVER_H_
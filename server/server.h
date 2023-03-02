#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include <mutex> // Библиотека для создания безопсной зоны потока
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

#include <chrono> // Либа для засыпания программы

bool m_shutdown_server_ = false;
constexpr int SERVER_PORT = 87;
const char* HELLO_CLIENT = "Hello! You got it! \n";
const char* ERROR_REG = "ERROR: It was not possible to generate regular expressions for the client's request.\n Connection interrupted \n";
const char* INCORECT_REQUEST_NON_SEQ = "WARNING: The request was made incorrectly.\nMore than one seq has not been set with the correct parameters,\nnumber generation is impossible \n";
const char* INCORECT_REQUEST = "WARNING: The request was incorrect \n";

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
                * Функция проверки наличия новых подключений

                * Основной метод, вызывающий poll()
            */ 
            void CheckingConnectionRequests();

            /*
                * Функция обработки сигнала Ctrl + C

                Incoming:
                    signum - входной сигнал
            */ 
            static void SigHandler(int signum);


        private:
            
            /*
                * Обрабатывает события конкретного клиента

                * Вызывает Disconnect User(), 
                когда получает запрос на отключение.

                Incoming:
                    fd - дискриптор клиента
            */
            void RequestProcessing(int fd);

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

            // Удаление дискриптора клиента из списка дискрипторв
            void CompressArray();

            // Масив дескрипторов клиента
            pollfd m_cfds_[5]{};
            // Буфер запросов клиента
            char buf[BUFF_SIZE];
            // Слушающий дискриптор
            pollfd m_fds_[1]{};
            // Счетчик количества отслеживаемых дискрипторов
            int m_fds_counter_;
            // Объявление мютекса
            std::mutex mutex_;

    }; // class Server
} // namespace net_protocol

#endif // SERVER_SERVER_H_
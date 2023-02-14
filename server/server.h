#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include <algorithm> //набор функций для выполнения алгоритмических операций 
                    //над контейнерами и над другими последовательностями
#include <vector> // Реализация динамического массива
#include <csignal> // Макросы сигналов и функции их отработки
#include <fstream> // набор классов, методов и функций, которые предоставляют 
                  // интерфейс для чтения/записи данных из/в файл
#include <queue> // Реализация очереди

#include "../NetProcessing/NetProcessing.h" // Оебртка стандартных методов

bool m_shutdown_server_ = false;
constexpr int SERVER_PORT = 8080;

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

            // Диструктор класса сервер
            ~Server();

            /*
                * Функция обработки сигнала Ctrl + C

                Incoming:
                    signum - входной сигнал
            */ 
            static void SigHandler(int signum);
            
            /*
                * Основная функция регулирующая работу сервера

                * Основной метод, вызывающий poll()

                * Eсли у какого-либо клиента есть какой-либо запрос, 
                вызывает EventProcessing()
            */ 
            void HandlingCycle();

        private:
            // Функция ужатия масива дискрипторов клиентов
            void CompressArray();

            /*
                * Обрабатывает события из всех полей

                * Вызывает Disconnect User(), 
                когда получает запрос на отключение.
            */
            void EventProcessing();

            // Инициализация и настройка порта для подключения
            void InitListenPorts();

            // Буфер клиента
            char buf[BUFF_SIZE];
            // Список отслеживаемых файловых дискрипторов
            pollfd m_fds_[7]{};
            // Счетчик количества отслеживаемых файловых дискрипторов
            int m_fds_counter_;
            // Битовая длина адресса
            auto addr_size;
            // Серверный сокет
            int serverSocket;
            // Добавляемый сокет
            int NewSocket;
            // Флаг на ужатие массива файловых дискрипторов
            bool m_compress_arr_;

    }; // class Server
} // namespace net_protocol

#endif // SERVER_SERVER_H_
#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <stdexcept>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdint>
#include <functional>
#include <thread>
#include <list>
#include <clickhouse/client.h>
#include <clickhouse/error_codes.h>
#include <clickhouse/types/type_parser.h>
#include "../headers/dbaction.h"



#include <stdexcept>
#include <iostream>
#include <vector>
#include <cmath>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <clickhouse/client.h>
#include <clickhouse/error_codes.h>
#include <clickhouse/types/type_parser.h>



//Буффер для приёма данных от клиента
static constexpr uint16_t buffer_size = 4096;
static constexpr uint16_t temp_buff = 2048;

struct TcpServer {
    class Client;
     //Тип Callback-функции обработчика клиента
    typedef std::function<void(Client)> handler_function_t;
     //Статус сервера
    enum class status : uint8_t {
        up = 0,
        err_socket_init = 1,
        err_socket_bind = 2,
        err_socket_listening = 3,
        close = 4
    };

private:
    uint16_t port; //Порт
    status _status = status::close;
    handler_function_t handler;

    std::thread handler_thread;
    std::list<std::thread> client_handler_threads;
    std::list<std::thread::id> client_handling_end;

    int serv_socket;

    void handlingLoop();

public:
    TcpServer(const uint16_t port, handler_function_t handler);
    ~TcpServer();

    //! Set client handler
    void setHandler(handler_function_t handler);

    uint16_t getPort() const;
    uint16_t setPort(const uint16_t port);

    status getStatus() const {return _status;}

    status restart();
    status start();
    void stop();

    void joinLoop();
};

class TcpServer::Client {
    int socket;
    struct sockaddr_in address;
    char buffer[buffer_size];
	int headerEnd;
	int sizeData;
	std::map<std::string, std::string> params;
	std::vector<std::string> lang;

public:
    Client(int socket, struct sockaddr_in address);
    Client(const Client& other);
    ~Client();
    uint32_t getHost() const;
    uint16_t getPort() const;

    int loadData();
    char* getData(int);
	std::map<std::string, std::string>* parseData(const std::string hostIp, const std::string hostPort);
    bool sendData(const char* buffer, const size_t size) const;
	std::vector<std::string> *getLang();
	void cleanData();
	void __dumpData();
	void debugKill();
};

#endif // TCPSERVER_H

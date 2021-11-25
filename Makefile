 all: server main.cpp
	g++ -std=c++20 -g3 -Wall -pthread main.cpp -o dummyhouse TcpServer.o -lclickhouse-cpp-lib
 server: source/TcpServer.cpp
	g++ -std=c++20 -Wall -pthread -c -g3  source/TcpServer.cpp

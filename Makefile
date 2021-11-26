all: server dbaction main.cpp
	g++ -std=c++20 -g3 -Wall -pthread main.cpp -o dummyhouse TcpServer.o dbaction.o -lclickhouse-cpp-lib
dbaction: source/dbaction.cpp
	g++ -std=c++20 -Wall -pthread -c -g3  source/dbaction.cpp
server: source/TcpServer.cpp
	g++ -std=c++20 -Wall -pthread -c -g3  source/TcpServer.cpp

#ifndef TCPACTION_H
#define TCPACTION_H
#include "TcpServer.h"

using namespace clickhouse;
using namespace std;

class DataB {
private:
	Client client;
public:
	DataB(char *host, char *passwd);
	~DataB();
	void createTable();
	void insertTable();
};

#endif

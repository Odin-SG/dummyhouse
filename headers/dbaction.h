#ifndef TCPACTION_H
#define TCPACTION_H
#include "TcpServer.h"

using namespace clickhouse;
using namespace std;

class DataB {
private:
	Client client;
	void createTable();
public:
	DataB(char *host, char *passwd);
	~DataB();
	void insertTable(const std::map<std::string, std::string>* params, const std::vector<std::string> *langs);
};

#endif

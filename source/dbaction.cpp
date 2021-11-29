#include "../headers/dbaction.h"
void DataB::createTable(){
		client.Execute("CREATE TABLE IF NOT EXISTS test.hits (id UInt256, ip String, os String, browser String, timezone Int8, cookies String, prefer String) ENGINE = Memory");
}

DataB::DataB(char *host, char *passwd): client(ClientOptions().SetHost(host).SetPassword(passwd).SetPingBeforeQuery(true)) {createTable();}
DataB::~DataB(){
	client.Execute("DROP TABLE test.hits");
}

void DataB::insertTable(std::map<std::string, std::string>* params){
		client.Execute("");
}


/*DataB::DataB(char *host, char *passwd) {
	try {
		this->client = new Client(ClientOptions().SetHost(host).SetPassword(passwd).SetPingBeforeQuery(true)); //Удалить в деструкторе

	} catch (const exception &e) {
		cerr << "DataBase exception: " << e.what() << endl;
	}
	exit(1);
}*/

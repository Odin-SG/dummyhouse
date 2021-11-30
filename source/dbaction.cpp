#include "../headers/dbaction.h"
void DataB::createTable(){
    client.Execute("CREATE TABLE IF NOT EXISTS test.hits (session_id String, ip String, os String, browser String, timezone String, cookies String, prefer String) ENGINE = Memory");
}

DataB::DataB(char *host, char *passwd): client(ClientOptions().SetHost(host).SetPassword(passwd).SetPingBeforeQuery(true)) {createTable();}
DataB::~DataB(){
	//client.Execute("DROP TABLE test.hits");
}

void DataB::insertTable(const std::map<std::string, std::string>* params){
	Block block;

	auto session_id = std::make_shared<ColumnString>();
	try{
		session_id->Append(params->at("session_id"));
	} catch(std::out_of_range const&){
		std::cout << "'session_id' not exist key" << std::endl;
		session_id->Append("");
	}

	auto ip = std::make_shared<ColumnString>();
	try{
		ip->Append(params->at("ip"));
	} catch(std::out_of_range const&){
		std::cout << "'ip' not exist key" << std::endl;
		ip->Append("");
	}

	auto os = std::make_shared<ColumnString>();
	try{
		os->Append(params->at("os"));
	} catch(std::out_of_range const&){
		std::cout << "'os' not exist key" << std::endl;
		os->Append("");
	}

	auto browser = std::make_shared<ColumnString>();
	try{
		browser->Append(params->at("browser"));
	} catch(std::out_of_range const&){
		std::cout << "'browser' not exist key" << std::endl;
		browser->Append("");
	}

	auto timezone = std::make_shared<ColumnString>();
	try{
		timezone->Append(params->at("timezone"));
	} catch(std::out_of_range const&){
		std::cout << "'timezone' not exist key" << std::endl;
		timezone->Append("");
	}

	auto cookies = std::make_shared<ColumnString>();
	try{
		cookies->Append(params->at("cookies"));
	} catch(std::out_of_range const&){
		std::cout << "'cookies' not exist key" << std::endl;
		cookies->Append("");
	}

	auto prefer = std::make_shared<ColumnString>();
	try {
		prefer->Append(params->at("prefer"));
	} catch(std::out_of_range const&){
		std::cout << "'prefer' not exist key" << std::endl;
		prefer->Append("");
	}

	block.AppendColumn("session_id", session_id);
	block.AppendColumn("ip", ip);
	block.AppendColumn("os", os);
	block.AppendColumn("browser", browser);
	block.AppendColumn("timezone", timezone);
	block.AppendColumn("cookies", cookies);
	block.AppendColumn("prefer", prefer);

	client.Insert("test.hits", block);
}


/*DataB::DataB(char *host, char *passwd) {
	try {
		this->client = new Client(ClientOptions().SetHost(host).SetPassword(passwd).SetPingBeforeQuery(true)); //Удалить в деструкторе

	} catch (const exception &e) {
		cerr << "DataBase exception: " << e.what() << endl;
	}
	exit(1);
}*/

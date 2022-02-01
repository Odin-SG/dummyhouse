#include "../headers/dbaction.h"
void DataB::createTable(){
    client.Execute("CREATE TABLE IF NOT EXISTS dummyhouse.hits (session_id String, ip String, port String, os String, browser String, timezone String, cookies String, prefer String, language String, languages Array(String), fingerprint String) ENGINE = Memory");
}

DataB::DataB(char *host, char *passwd): client(ClientOptions().SetHost(host).SetPassword(passwd).SetPingBeforeQuery(true)) {createTable();}
DataB::~DataB(){
	//client.Execute("DROP TABLE dummyhouse.hits");
}

char DataB::intArrStrings(clickhouse::Block *block, std::string *name, const std::map<std::string, std::string> *params, const std::vector<std::string> *langs){
	auto arr = std::make_shared<ColumnArray>(std::make_shared<ColumnString>());
	auto elemArr = std::make_shared<ColumnString>();
	char success = 1;
	try {
		for(auto const &elem: *langs){
			std::cout << elem << "-<" << std::endl;
			elemArr->Append(elem);
		}
		arr->AppendAsColumn(elemArr);
		success = 0;
	} catch(std::out_of_range const&){
		std::cout << "'languages' not exist key" << std::endl;
		elemArr->Append("");
		arr->AppendAsColumn(elemArr);
	}
	block->AppendColumn(*name, arr);
	return success;
}


char DataB::insString(clickhouse::Block *block, string *name, const std::map<std::string, std::string> *params) {
	auto colom = std::make_shared<ColumnString>();
	char success = 1;
	try{
		colom->Append(params->at(*name));
		success = 0;
	} catch(std::out_of_range const&){
		stringstream error;
		error <<  "'" << *name << "'  not exist key";
		std::cout << error.str() << std::endl;
		colom->Append("");
	}
	block->AppendColumn(name->c_str(), colom);
	return success;
}

void DataB::insertTable(const std::map<std::string, std::string> *params, const std::vector<std::string> *langs){
	Block block;
	int colums = 7;

	map<string, int> coloms = {
		{"session_id", 1},
		{"ip", 1},
		{"port", 1},
		{"os", 1},
		{"browser", 1},
		{"timezone", 1},
		{"cookies", 1},
		{"prefer", 1},
		{"language", 1},
		{"languages", 2},
		{"fingerprint", 1},
	};

	for(auto const &colom: coloms){
		if(colom.second == 1){
			string name = colom.first;
			insString(&block, &name, params);
		}
		if(colom.second == 2){
			string name = colom.first;
			intArrStrings(&block, &name, params, langs);
		}
	}

	/*if(colums > 0){
		block.AppendColumn("session_id", session_id);
		block.AppendColumn("ip", ip);
		block.AppendColumn("port", port);
		block.AppendColumn("os", os);
		block.AppendColumn("browser", browser);
		block.AppendColumn("timezone", timezone);
		block.AppendColumn("cookies", cookies);
		block.AppendColumn("prefer", prefer);
		block.AppendColumn("language", language);
		block.AppendColumn("languages", languages);
		block.AppendColumn("fingerprint", fingerprint);*/

	client.Insert("dummyhouse.hits", block);
	//}
}


/*DataB::DataB(char *host, char *passwd) {
	try {
		this->client = new Client(ClientOptions().SetHost(host).SetPassword(passwd).SetPingBeforeQuery(true)); //Удалить в деструкторе

	} catch (const exception &e) {
		cerr << "DataBase exception: " << e.what() << endl;
	}
	exit(1);
}*/

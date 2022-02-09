#include "../headers/TcpServer.h"
#include <chrono>
#include <map>

//Конструктор принимает:
//port - порт на котором будем запускать сервер
//handler - callback-функция запускаямая при подключении клиента
//          объект которого и передают первым аргументом в callback
//          (пример лямбда-функции: [](TcpServer::Client){...do something...})
TcpServer::TcpServer(const uint16_t port, handler_function_t handler) : port(port), handler(handler) {}

//Деструктор останавливает сервер если он был запущен
//и вычищает заданную версию WinSocket
TcpServer::~TcpServer() {
	if(_status == status::up)
		stop();
}

//Задаёт callback-функцию запускаямую при подключении клиента
void TcpServer::setHandler(TcpServer::handler_function_t handler) {this->handler = handler;}

//Getter/Setter порта
uint16_t TcpServer::getPort() const {return port;}
uint16_t TcpServer::setPort( const uint16_t port) {
	this->port = port;
	restart(); //Перезапустить если сервер был запущен
	return port;
}

//Перезапуск сервера
TcpServer::status TcpServer::restart() {
	if(_status == status::up)
		stop ();
	return start();
}

// Вход в поток обработки соединений
void TcpServer::joinLoop() {handler_thread.join();}

//Загружает в буфер данные от клиента и возвращает их размер
int TcpServer::Client::loadData() {return recv(socket, buffer, buffer_size, 0);}

//Возвращает указатель на буфер с данными от клиента и пишем в поле длинну заголовков
char* TcpServer::Client::getData(int size) {
	int hStop = size;
	char urlBuffer[64];
	for(int i = 0; i < size; i++){
		if(buffer[i] == '\n' && buffer[i-1] == '\r' && buffer[i+2] == '\n' && buffer[i+1] == '\r'){
			hStop = i + 3;
		}
		if(!i){
			for(int j = 0; j < 64; j++){
				if(buffer[j] == '\n')
					break;
				urlBuffer[j] = buffer[j];
			}
		}
	}

	headerEnd = hStop + 1;
	sizeData = size;
	return buffer;
}

using namespace std;

//Парсим данные. Те, что после заголовков.
std::map<std::string, std::string>* TcpServer::Client::parseData(const std::string hostIp, const std::string hostPort) {
	int startName = headerEnd, endName = headerEnd;
	int startVal = headerEnd, endVal = headerEnd;
	char tempBufName[temp_buff], tempBufVal[temp_buff];
	bool nameEnd = false;
	params["ip"] = hostIp;
	params["port"] = hostPort;
	bool paramExist = false;

	for(int pos = headerEnd; pos < sizeData; pos++){
		if(buffer[pos] == ':' && !nameEnd){
			endName = pos;
			memset(tempBufName, 0, temp_buff);
			memcpy(tempBufName, &buffer[startName], endName - startName);
			nameEnd = true; paramExist = true;
			//cout << "<" << tempBufName << "> =";
		}

		if(buffer[pos] == '\\' && buffer[pos+1] == 'n' && paramExist == true){
			startVal = endName+1;
			endVal = pos;

			memset(tempBufVal, 0, temp_buff);
			memcpy(tempBufVal, &buffer[startVal], endVal - startVal);
			//strcat(tempBufVal, "");
			//cout << tempBufName << "= <" << strlen(tempBufVal) << ">" << endl;
			startName = pos+2;

			//Создаём массив, где будем хранить раскладки пользователя
			if(strcmp(tempBufName, "languages") == 0) {
					std::string name(tempBufVal), val;
					char delimiter = ';';
					size_t pos;
					while((pos = name.find(delimiter)) <= strlen(tempBufVal)){
						val = name.substr(0, pos);
						lang.push_back(val);
						name.erase(0, pos + 1);
					}
			}
			params[tempBufName] = tempBufVal;
			nameEnd = false; paramExist = false;
		}
	}

	return &params;
}

void TcpServer::Client::cleanData(){
	memset(buffer, 0, sizeof(buffer));
}

void TcpServer::Client::__dumpData(){
		map<string, string>::iterator it;

		for (it = params.begin(); it != params.end(); ++it)
			cout << "->" << it->first << " " << it->second << '\n';
}

std::vector<std::string> *TcpServer::Client::getLang(){
	return &lang;
}

void TcpServer::Client::debugKill(){
	int locHeaderEnd = headerEnd - 1 ;
	string killString = "000xxxFDASTPCODESIGKILL";
	if((sizeData - locHeaderEnd)+1 >= (int)killString.length()){
		char *dataSector = &buffer[locHeaderEnd];
		if(dataSector == killString){
				exit(0);
		}
	}
}


//Отправляет данные клиенту
bool TcpServer::Client::sendData(const char* buffer, const size_t size) const {
	if(send(socket, buffer, size, 0) < 0) return false;
	return true;
}


//Запуск сервера (по аналогии с реализацией для Windows)
TcpServer::status TcpServer::start() {
	struct sockaddr_in server;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( port );
	server.sin_family = AF_INET;
	serv_socket = socket(AF_INET, SOCK_STREAM, 0);
	int flagReuseaddr = 1;

	setsockopt(serv_socket, SOL_SOCKET, SO_REUSEADDR, &flagReuseaddr, sizeof(flagReuseaddr));
	if(serv_socket == -1) return _status = status::err_socket_init;
	if(bind(serv_socket,(struct sockaddr *)&server , sizeof(server)) < 0) return _status = status::err_socket_bind;
	if(listen(serv_socket, 3) < 0)return _status = status::err_socket_listening;

	_status = status::up;
	handler_thread = std::thread([this]{handlingLoop();});
	return _status;
}

//Остановка сервера
void TcpServer::stop() {
	_status = status::close;
	close(serv_socket);
	joinLoop();
	for(std::thread& cl_thr : client_handler_threads)
		cl_thr.join();
	client_handler_threads.clear ();
	client_handling_end.clear ();
}

// Функиця обработки соединений (по аналогии с реализацией для Windows)
void TcpServer::handlingLoop() {
	while (_status == status::up) {
		int client_socket;
		struct sockaddr_in client_addr;
		int addrlen = sizeof (struct sockaddr_in);
		if((client_socket = accept(serv_socket, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen)) >= 0 && _status == status::up)
			client_handler_threads.push_back(std::thread([this, &client_socket, &client_addr] {
				handler(Client(client_socket, client_addr));
				client_handling_end.push_back (std::this_thread::get_id());
			}));

		if(!client_handling_end.empty())
			for(std::list<std::thread::id>::iterator id_it = client_handling_end.begin (); !client_handling_end.empty() ; id_it = client_handling_end.begin())
				for(std::list<std::thread>::iterator thr_it = client_handler_threads.begin (); thr_it != client_handler_threads.end () ; ++thr_it)
					if(thr_it->get_id () == *id_it) {
						thr_it->join();
						client_handler_threads.erase(thr_it);
						client_handling_end.erase (id_it);
						break;
					}
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

// Конструктор клиента по сокету и адресу
TcpServer::Client::Client(int socket, struct sockaddr_in address) : socket(socket), address(address) {}
// Конструктор копирования
TcpServer::Client::Client(const TcpServer::Client& other) : socket(other.socket), address(other.address) {}

TcpServer::Client::~Client() {
	shutdown(socket, 0); //Обрыв соединения сокета
	close(socket); //Закрытие сокета
}

// Геттеры хоста и порта
uint32_t TcpServer::Client::getHost() const {return address.sin_addr.s_addr;}
uint16_t TcpServer::Client::getPort() const {return address.sin_port;}


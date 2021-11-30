#include "headers/TcpServer.h"

#include <iostream>



//Парсер ip в std::string
std::string getHost(const TcpServer::Client& client) {
  uint32_t ip = client.getHost ();
  return std::string() + std::to_string(reinterpret_cast<unsigned char*>(&ip)[0]) + '.' +
         std::to_string(reinterpret_cast<unsigned char*>(&ip)[1]) + '.' +
         std::to_string(reinterpret_cast<unsigned char*>(&ip)[2]) + '.' +
         std::to_string(reinterpret_cast<unsigned char*>(&ip)[3]) + ':' +
         std::to_string( client.getPort ());
}

int main(int argc, char *argv[]) {
//Создание объекта TcpServer с передачей аргументами порта и лябда-фунции для обработк клиента
TcpServer server( 8080,
		[](TcpServer::Client client){
			//Ожидание данных от клиента
			int size = 0;
			int tryCount = 0;
			bool success = true;
			char *data;
			std::string hostIp;
						//Вывод адреса подключившего клиента в консоль
			hostIp = getHost(client);
			std::cout<<"Connected host:"<<hostIp<<std::endl;

			while (!size){
				if(tryCount <= 100){
					size = client.loadData ();
					tryCount++;
				} else {
					success = false;
					break;
				}
			};
			if(success){
				//std::cout<<std::endl<<"size: "<<size<< std::endl;
				//Вывод размера данных и самих данных в консоль
				DataB database("localhost", "123");
				data = client.getData(size);

				std::cout << "Bytes:"<<std::endl << data << std::endl;
				std::map<std::string, std::string>* parsedData = client.parseData(hostIp);

				client.__dumpData();
				database.insertTable(parsedData);

				//Отправка ответа клиенту
				const char answer[] = "Hello World from Server";
				client.sendData(answer, sizeof (answer));
			}
			client.cleanData();
		}
);

  //Запуск серевера
  if(server.start() == TcpServer::status::up) {
    //Если сервер запущен вывести сообщение и войти в поток ожиданий клиентов
      std::cout<<"Server is up!"<<std::endl;
      server.joinLoop();
  } else {
    //Если сервер не запущен вывод кода ошибки и заверешение программы
      std::cout<<"Server start error! Error code:"<< int(server.getStatus()) <<std::endl;
      return -1;
  }

}

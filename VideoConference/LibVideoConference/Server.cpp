#include "Server.h"
using namespace std;

Server::Server(QWidget *parent)
{
	std::cout << "Server is open!" << std::endl;
	server.setIP("127.0.0.1").setPort(6666);
	server.start();
	guis.show();
	
	connect(&timer, SIGNAL(timeout()), this, SLOT(onTimeout()));

	t = new QThread;
	timer.start(1000);
	timer.moveToThread(t);
	//t->start();
}
Server::~Server()
{
	server.stop();
}
#include <../LibVideoConference/Server.h>
#include <QtWidgets/QApplication>
#include"../LibVideoConference/CIPFinder.h"
int main(int argc, char *argv[])
{
	std::cout << "Server is open!" << std::endl;
    QApplication a(argc, argv);
	SockServer server("0.0.0.0:356");
	CIPFinder ipfinder(1);
	guiServer guis;
	server.start();
	
	guis.show();
	CSQLService sql;

    return a.exec();
}

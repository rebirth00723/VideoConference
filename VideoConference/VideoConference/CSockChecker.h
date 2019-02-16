#ifndef CSOCKCHECKER_H
#define CSOCKCHECKER_H

#include"UDPSocket.h"
#include"qwidget.h"
#include "set"
#include <../LibVideoConference/SQLService.h>
class CSockChecker :public QWidget{
	Q_OBJECT
public:

	std::map<int, CUserData>* getMemList();
	void					  setMemList(map<int, CUserData>);
	bool					  hasUser(int uid);
	CSockChecker(int id);
	char* recvdVip;
	char* recvdAip;
	char* recvdSip;
	int recvdId;
	char* ip;

	int myid;

	
private:
	std::map<int, CUserData> udList;
signals:
	void newConnect();
};


#endif
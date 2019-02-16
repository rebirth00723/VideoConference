#ifndef CIPFINDER_H
#define CIPFINDER_H

#include"UDPSocket.h"
#include"qwidget.h"
#include"qhostinfo.h"
#include"qnetworkinterface.h"
#include"qnetworkreply.h"
#include"qnetworkaccessmanager.h"
#include"qnetworkrequest.h"
#include"qmessagebox.h"
#include<string.h>
#include<iostream>
#include<queue>

using namespace std;
//queue<string>recvIPList;
class CIPFinder :public QWidget{
	Q_OBJECT
private:
	QString QExternalIP;
	string ExternalIP;	

public:
	CIPFinder(bool sendMyIP);
	string iplist[100];
	QString QLanIP;
	string LanIP;
	string recvIP;


	string getMyHostIP();

	string* ipListPtr();
	QNetworkAccessManager *accessManager;

	int ipcnt=0;

public slots:
	void processWhatMyIPReply(QNetworkReply *reply);
signals:
	void newConnect();
};


#endif
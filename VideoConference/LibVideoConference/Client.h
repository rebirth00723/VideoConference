#pragma once
#ifndef CLIENT_H
#define CLIENT_H

#include "qdebug.h"
#include "qthread.h"
#include "qtimer.h"
#include "guiLogin.h"
#include "guiClient.h"

#include "../LibScreenShare/Network.h"
class Client : public QObject
{
	Q_OBJECT

public:
	Client(QWidget *parent = Q_NULLPTR);
	
private:
	SockClient			client;
	guiLogin			guiLogin;
	guiCPackage			guiCPackage;
	guiClient			*guiclient;
	guiOID				*guioID;
	guiSP				*guisP;
	guiMID				*guimID;
	map<int, CMeetingData>CData;
	
private slots:
	void			show_Mainwindow(int);
};
#endif
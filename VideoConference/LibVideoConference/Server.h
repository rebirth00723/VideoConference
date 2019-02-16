#pragma once
#ifndef SERVER_H
#define SERVER_H

#include "qdebug.h"
#include "qthread.h"
#include "qtimer.h"
#include "guiServer.h"
#include"../LibScreenShare/Network.h"
class Server : public QObject
{
	Q_OBJECT

public:
	Server(QWidget *parent = Q_NULLPTR);
	~Server();
private:
	guiServer guis;
	SockServer server;
	QThread *t;
	QTimer timer;

private slots:
	void onTimeout();

};
#endif

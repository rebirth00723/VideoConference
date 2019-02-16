#pragma once
#ifndef GUILOGIN_H
#define GUILOGIN_H

#include "guiED.h"
#include "ColorDefine.h"
#include <../LibScreenShare/Network.h>
#include <../LibVideoConference/guiClient.h>
#include"../LibVideoConference/CIPFinder.h"
class guiLogin : public QWidget
{
	Q_OBJECT

public:
	guiLogin(CMediaSender *, CMediaSender *, CMediaSender *, QWidget *parent = Q_NULLPTR);
	void setSock(SockClient *client)
	{
		this->client = client;
	}
	void setGuiClient(guiClient **gc)
	{
		_guiClient = gc;
	}
private:


	CMediaSender *A;
	CMediaSender *M;
	CMediaSender *D;

	CIPFinder			*ipfinder;
	guiED				guiED;
	ColorDefine			colorDefine;
	//CSQLService			sql;
	SockClient			*client;
	QPalette			blackBG;
	QPalette			orangeText;
	QString				btnStyleSheet;

	QLineEdit			*account;
	QLineEdit			*password;
	QPushButton			*login_btn;
	QVBoxLayout			*vbox;

	void				createClient(int uid);
	void				login_Widget();
	void				login_Layout();
	guiClient			**_guiClient;
	char				toChar(QString);

private slots:
	void				sqlcheck();
	void				showGuiLogin();

};
#endif
#pragma once 
#ifndef GUIMID_H
#define GUIMID_H

#include "qdialog.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qlayout.h"
#include "qtextbrowser.h"
#include "qdatetime.h"
#include "qdebug.h"
#include "qfile.h"

#include "guiED.h"
#include "SQLService.h"
#include"../VideoConference/ConferenceUI.h"
#include <../LibScreenShare/Network.h>

#include <iostream>
#include <map>
#define qs QStringLiteral
class guiMID :public QDialog
{
	Q_OBJECT

public:
	guiMID(int uid, SockClient *sock,QWidget *parent = Q_NULLPTR);
	void	getMapdata(std::map<int, CUserData>*);
	
	int myID; //Nei Addd
private:
	ConferenceUI	*cui;
	SockClient		*client;
	CUserData_arr			member_data;
	map<int, CUserData>    *UIDData;
	map<int, CUserData>		udList;
	int				count;
	int				uid;
	int				mid;
	ColorDefine		colordefine;

	QPalette		blackBG;
	QPalette		orangeText;
	QPalette		blackBase;
	QString			btnStyleSheet;

	QLabel			*name_t;
	QLabel			*date_t;
	QLabel			*issue_t;
	QLabel			*creater_t;
	QLabel			*staff_t;

	QLabel			*name;
	QLabel			*date;
	QTextBrowser	*issue;
	QLabel			*creater;
	QLabel			*staff;

	QPushButton		*enter;
	QPushButton		*leave;

	QHBoxLayout		*hbox1;
	QHBoxLayout		*hbox2;
	QHBoxLayout		*hbox3;
	QHBoxLayout		*hbox4;
	QHBoxLayout		*hbox5;
	QVBoxLayout		*vbox1;

	void			information_Widget();
	void			information_Layout();

	QString			timeStampToQString(int);
private slots:

	void			showMID(CMeetingData);
	void			enter_clicked();
	void			leaveConference(int);
signals:
	void			setMeeting(int);
};
#endif
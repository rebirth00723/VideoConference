#pragma once 
#ifndef GUIOID_H
#define GUIOID_H

#include "qdialog.h"
#include "qtoolbox.h"
#include "qwidget.h"
#include "qlayout.h"
#include "qlabel.h"
#define qs QStringLiteral

class guiOID :public QDialog
{
	Q_OBJECT

public:
	guiOID(QWidget *parent = Q_NULLPTR);

private:
	QPalette		blackBackground;

	QToolBox		*toolbox;
	
	QWidget			*page1;
	QWidget			*page2;
	QWidget			*page3;
	QWidget			*page4;
	QWidget			*page5;
	QWidget			*page6;

	QLabel			*text1;
	QLabel			*text2;
	QLabel			*text3;
	QLabel			*text4;
	QLabel			*text5;
	QLabel			*text6;

	QVBoxLayout		*textbox1;
	QVBoxLayout		*textbox2;
	QVBoxLayout		*textbox3;
	QVBoxLayout		*textbox4;
	QVBoxLayout		*textbox5;
	QVBoxLayout		*textbox6;
	QVBoxLayout		*vbox;
private slots :
	void show_operatingInformation();
};
#endif
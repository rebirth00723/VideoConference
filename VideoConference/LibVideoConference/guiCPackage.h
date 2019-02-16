#pragma once
#ifndef GUICPACKAGE_H
#define GUICPACKAGE_H

#include <QtWidgets/QWidget>
#include "qwidget.h"
#include "qframe.h"
#include "qlabel.h"
#include "qpushbutton.h"
#include "qlayout.h"
#include "qdebug.h"
#include "CPushButton.h"
#define qs QStringLiteral
class guiCPackage : public QFrame
{
	Q_OBJECT

public:
	guiCPackage(QWidget *parent = Q_NULLPTR);
	void				newFrame(QFrame*, int, QString ,QString, int);

	QLabel				*status;
	QLabel				*nameTitle;
	QLabel				*timeTitle;
	QLabel				*name;
	QLabel				*time;
	QPushButton			*inBtn;
	QString				btnstylesheet;
	QString				statusG;
	QString				statusR;
	CPushButton			*innbtn;

	QGridLayout			*gbox;
	QHBoxLayout			*hbox;

signals:
	void			 showMID(int);

private slots:
	void			 onBtnClicked();

};
#endif

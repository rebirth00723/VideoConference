#pragma once
#ifndef GUIED_H
#define GUIED_H

#include "qdialog.h"
#include "qwidget.h"
#include "qpushbutton.h"
#include "qlabel.h"
#include "qlineedit.h"
#include "qlayout.h"
#include "qmessagebox.h"
#include "qtimer.h"
#include <QRegExpValidator>
#include "qdebug.h"
#include "ColorDefine.h"
#define qs QStringLiteral
class guiED : public QDialog
{
	Q_OBJECT

public:
	guiED(QDialog *parent = Q_NULLPTR);

	void				EventDialog(int, int err = 0);
private:
	ColorDefine			colorDefine;

	QLabel				*message;
	QPushButton			*btn;
	QPushButton			*signoutBtn;
	QSpacerItem			*hSpacer;

	QVBoxLayout			*vbox;
	QHBoxLayout			*hbox;

	QPalette			blackBG;
	QPalette			orangeText;
	QString				btnStyleSheet;

	
private slots:
	void				signout();
signals:
	void				signout_Success();
};
#endif
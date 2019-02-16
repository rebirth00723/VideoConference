#pragma once
#ifndef GUISP_H
#define GUISP_H

#include "guiED.h"
#include "ColorDefine.h"
#include <QtWidgets/QWidget>
#include <QRegExpValidator>
#include "qlineedit.h"
#include "qmessagebox.h"

class guiSP : public QDialog
{
	Q_OBJECT

public:
	guiSP(QWidget *parent = Q_NULLPTR);

private:
	guiED			guiED;
	ColorDefine		colordefine;

	QPalette		blackBackground;
	QPalette		orangeText;
	QPalette		orangeWindowText;
	QString			btnStyleSheet;

	QLabel			*oldPasswordTitle;
	QLineEdit		*oldPassword;
	QLabel			*newPasswordTitle;
	QLineEdit		*newPassword1;
	QLineEdit		*newPassword2;
	QPushButton		*confirm;

	QSpacerItem		*hSpacer;
	QVBoxLayout		*vbox;

	void			setPassword_Widget();
	void			setPassword_Layout();

private slots :
	void			show_setPassword();
	void			setPasswordCheck();
	
};
#endif
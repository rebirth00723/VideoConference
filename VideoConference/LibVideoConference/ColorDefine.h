#pragma once
#ifndef COLORDEFINE_H
#define COLORDEFINE_H
#include "qobject.h"
#include "qpalette.h"

class ColorDefine : public QObject
{
	Q_OBJECT

public:
	ColorDefine(QWidget *parent = Q_NULLPTR);

	void		setEDMIDColor(QPalette*,QPalette*,QString*);
	void		setLoginColor(QPalette*, QPalette*, QString*);
	void		setSPColor(QPalette*, QPalette*, QPalette*, QString*);
	void		setClientColor(QPalette*, QPalette*, QPalette*, QPalette*, QPalette*, QString*, QString*, QString*, QString*, QString*);
	
	QString		btnstylesheet;
	QString		tabStyle;
	QString		tabWidget;
};
#endif
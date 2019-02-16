#pragma once
#ifndef CPUSHBUTTON_H
#define CPUSHBUTTON_H

#include <QtWidgets/QWidget>
#include "qpushbutton.h"

class CPushButton : public QPushButton
{
	Q_OBJECT

public:
	CPushButton(QWidget *parent = Q_NULLPTR);
	int btnID;
};
#endif

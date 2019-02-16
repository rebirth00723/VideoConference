#include "CPushButton.h"
using namespace std;
CPushButton::CPushButton(QWidget *parent) : QPushButton(parent)
{
	this->setText(QStringLiteral("會議資訊"));
	this->setFixedHeight(30);
	this->setFixedWidth(70);
}

#include "CPushButton.h"
using namespace std;
CPushButton::CPushButton(QWidget *parent) : QPushButton(parent)
{
	this->setText(QStringLiteral("�|ĳ��T"));
	this->setFixedHeight(30);
	this->setFixedWidth(70);
}

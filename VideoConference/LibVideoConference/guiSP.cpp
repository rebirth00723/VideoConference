#include "guiSP.h"
using namespace std;

guiSP::guiSP(QWidget *parent)
: QDialog(parent)
{
	colordefine.setSPColor(&blackBackground, &orangeWindowText, &orangeText, &btnStyleSheet);

	this->setWindowTitle(qs("���K�X"));
	this->setFixedSize(400, 300);
	this->setFont(QFont("", 12, QFont::Black));
	this->setPalette(blackBackground);

	setPassword_Widget();
	setPassword_Layout();

	connect(confirm, SIGNAL(clicked()), this, SLOT(setPasswordCheck()));
}

void guiSP::show_setPassword()
{
	this->exec();
}
void guiSP::setPassword_Widget()
{
	oldPasswordTitle = new QLabel(qs("�{�b�ϥΪ��K�X"));
	oldPasswordTitle->setPalette(orangeWindowText);
	oldPasswordTitle->setFont(QFont("", 14, QFont::Bold));
	newPasswordTitle = new QLabel(qs("�п�J�s���K�X"));
	newPasswordTitle->setPalette(orangeWindowText);
	newPasswordTitle->setFont(QFont("", 14, QFont::Bold));

	QRegExp rx("[a-zA-Z0-9]+$");
	QValidator *validator1 = new QRegExpValidator(rx, this);

	oldPassword = new QLineEdit();
	oldPassword->setPlaceholderText(qs("�п�J�ثe�ϥΪ��K�X"));
	oldPassword->setEchoMode(QLineEdit::Password);
	oldPassword->setMaxLength(16);
	oldPassword->setValidator(validator1);
	oldPassword->setStyleSheet("background:#000000;");
	oldPassword->setPalette(orangeText);
	oldPassword->setFrame(false);

	newPassword1 = new QLineEdit();
	newPassword1->setPlaceholderText(qs("�п�J6~16�줧�����b�έ^�Ʀr"));
	newPassword1->setEchoMode(QLineEdit::Password);
	newPassword1->setMaxLength(16);
	newPassword1->setValidator(validator1);
	newPassword1->setStyleSheet("background:#000000;");
	newPassword1->setPalette(orangeText);
	newPassword1->setFrame(false);

	newPassword2 = new QLineEdit();
	newPassword2->setPlaceholderText(qs("�T�{�K�X"));
	newPassword2->setEchoMode(QLineEdit::Password);
	newPassword2->setMaxLength(16);
	newPassword2->setValidator(validator1);
	newPassword2->setStyleSheet("background:#000000;");
	newPassword2->setPalette(orangeText);
	newPassword2->setFrame(false);

	confirm = new QPushButton(qs("�T�{"));
	confirm->setStyleSheet(btnStyleSheet);
	confirm->setFont(QFont("", 14, QFont::Black));
	
	
}
void guiSP::setPassword_Layout()
{
	vbox = new QVBoxLayout(this);
	vbox->setContentsMargins(30, 20, 30, 20);
	vbox->addWidget(oldPasswordTitle);
	vbox->addWidget(oldPassword);
	vbox->addWidget(newPasswordTitle);
	vbox->addWidget(newPassword1);
	hSpacer = new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);
	vbox->addItem(hSpacer);
	vbox->addWidget(newPassword2);
	hSpacer = new QSpacerItem(0, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);
	vbox->addItem(hSpacer);
	vbox->addWidget(confirm);
}

void guiSP::setPasswordCheck()
{
	if (!(oldPassword->text()).isEmpty() && !(newPassword1->text()).isEmpty() && !(newPassword1->text()).isEmpty())
	{
		int s = 1;

		/*���P�_�±K�X*/
		if (s == 1)
		{				
			/*�A�P�_��ӷs�K�X*/
			if (newPassword1->text() == newPassword2->text())
			{
				/*��s�K�X�s�J��Ʈw*/
				guiED.EventDialog(11);
				oldPassword->clear();
				newPassword1->clear();
				newPassword2->clear();
				this->close();
			}
			else
			{
				guiED.EventDialog(12);
				newPassword1->clear();
				newPassword2->clear();
			}
		}
		else
		{
			guiED.EventDialog(13);
			oldPassword->clear();
		}
	}
	else
		guiED.EventDialog(14);
	
}
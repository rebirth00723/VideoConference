#include "guiLogin.h"
using namespace std;

guiLogin::guiLogin(CMediaSender *A, CMediaSender *M, CMediaSender *D, QWidget *parent)
	: QWidget(parent)
{

	this->M = M;
	this->D = D;
	this->A = A;

	colorDefine.setLoginColor(&blackBG, &orangeText, &btnStyleSheet);

	this->setWindowTitle(qs("使用者登入"));
	this->setFixedSize(400, 300);
	this->setPalette(blackBG);

	login_Widget();
	login_Layout();

	connect(login_btn, SIGNAL(clicked()), this, SLOT(sqlcheck()));
}
void guiLogin::login_Widget()
{
	QRegExp rx("[a-zA-Z0-9]+$");
	QValidator *validator1 = new QRegExpValidator(rx, this);

	account = new QLineEdit();
	account->setFocus();
	account->setPlaceholderText(qs("UserID"));
	account->setEchoMode(QLineEdit::Normal);
	account->setMaxLength(16);
	account->setFont(QFont("", 14));
	account->setValidator(validator1);
	account->setStyleSheet("background:#000000;");
	account->setPalette(orangeText);
	account->setFrame(false);

	password = new QLineEdit();
	password->setPlaceholderText(qs("Password"));
	password->setEchoMode(QLineEdit::Password);
	password->setMaxLength(16);
	password->setFont(QFont("", 14));
	password->setValidator(validator1);
	password->setStyleSheet("background:#000000;");
	password->setPalette(orangeText);
	password->setFrame(false);

	login_btn = new QPushButton(qs("LOGIN"));
	login_btn->setStyleSheet(btnStyleSheet);
	login_btn->setFont(QFont("", 14, QFont::Black));
	login_btn->setAutoDefault(true);
}
void guiLogin::login_Layout()
{
	vbox = new QVBoxLayout(this);
	vbox->setContentsMargins(100, 80, 100, 100);
	vbox->addWidget(account);
	vbox->addWidget(password);
	vbox->addWidget(login_btn);
}
char guiLogin::toChar(QString str)
{
	string s = str.toStdString();
	const char* c = s.c_str();
	return *c;
}
void guiLogin::sqlcheck()
{
	bool ok;
	if (!(account->text()).isEmpty() && !(password->text()).isEmpty())
	{
		CPacketer pack;
		CodeBehavior code;
		std::string send;
		std::string recv;
		std::string ip;
		CUserData udata;

	//	ip = *(ipfinder->ipListPtr()) + ":356";
	//	cout << ip.c_str() <<endl;

			int id = (account->text()).toInt(&ok,10);
			std::string pwd = (password->text()).toUtf8().data();

			code = C_CHECK_USER;

			send = pack.Init().
				insert(&code).
				insert(&id).
				insert(pwd.c_str(), pwd.size(), true).
				getData();

			recv = client->sendCommand(send);
			cout << client->lastError() << endl;
			pack.Init(recv).pull(&code);

			if (code > 0)
				createClient(id);
			else
				guiED.EventDialog(1);
		
			password->setText("");
		

	}
	else
		guiED.EventDialog(2);
}
void guiLogin::showGuiLogin()
{
	guiED.EventDialog(3);
	(*_guiClient)->close();
	this->show();
}
void guiLogin::createClient(int uid)
{
	this->hide();
	guiED.EventDialog(0);
	(*_guiClient) = new guiClient(uid, client, A, D, M);

	QObject::connect(*_guiClient, SIGNAL(show_login()), this, SLOT(showGuiLogin()));
	(*_guiClient)->show();
	

}

#include"MyWindow.h"



static int cnt = 0;
static UDPSocket* FirstUDP;

void MyWindow::putt(){
	QString temp = text->toPlainText();
	temp += str;
	temp += '\n';

	text->setText(temp);
	str = "";
}

void recvv(void* param){

	struct arg{
		UDPSocket* mudp;
		MyWindow* mthis;
	};
	arg *marg = (arg*)param;


	char buf[BUFSIZE + 1];
	int len;
	while (1)
	{
		len = marg->mudp->recvfrom(buf, BUFSIZE);
		if (len < 1){
			printf("recv Error\n");
			printf("%d", WSAGetLastError());
			break;

		}
		buf[len] = '\0';
		if (len > 1)
		{
			printf("recv:%s\n", buf);
			printf("Thread:%d\n", cnt);
			marg->mthis->str = QString(buf);
			emit marg->mthis->gett();
		}
	}
}


MyWindow::MyWindow(char* ip, short port){
	cnt++;

	mip = ip;
	mport = port;
	
	/*
	bool isOK;
	//ID = QInputDialog::getText(NULL, "Input ID", "Input your ID", QLineEdit::Normal, "", &isOK);
	if (isOK)
	{
		QMessageBox::information(NULL, "Information", "Your ID is:<b>" + ID + "<b>", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
	}
	*/


	/*設定標題*/
	QString title = qs("USER:");
	title += ID;
	this->setWindowTitle(title);
	/**/

	text = new QTextEdit(this);
	line = new QLineEdit("type");
	sendBtn = new QPushButton(qs("發送"));

	vlayout = new QVBoxLayout(this);

	vlayout->addWidget(text);
	vlayout->addWidget(line);
	vlayout->addWidget(sendBtn);

	WSADATA wsad;
	WSAStartup(MAKEWORD(2, 2), &wsad);

	if (cnt == 1)
	{
		marg.mudp = new UDPSocket(mip, mport);
		FirstUDP = marg.mudp; //儲存第一次建立的UDP指標
	}
	else
	{
		marg.mudp = FirstUDP;
	}
	marg.mthis = this;

	QObject::connect(sendBtn, SIGNAL(clicked()), this, SLOT(gettext()));
	QObject::connect(this, SIGNAL(gett()), this, SLOT(putt()));



	_beginthread(recvv, 0, &marg);
}

void MyWindow::gettext()
{
	QString text = line->text();

	QString temp;
	temp += ID;
	temp += " : ";
	temp += text;

	QByteArray qbyte = temp.toUtf8();   //not toLocal8bit


	char *c = qbyte.data();

	printf("sent : %s\n", c);
	marg.mudp->sendto(c, BUFSIZE);

}
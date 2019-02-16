#include"CIPFinder.h"
CIPFinder* mthis;
char* CIPbuf = (char*)malloc(100);

void recv(void*p)
{
	UDPSocket*udp = (UDPSocket*)p;

	while (1)
	{
		int size =  udp->recvfrom(CIPbuf, 100);
		//cout << WSAGetLastError() << endl;
		string str(CIPbuf);
		mthis->recvIP = str;
		//cout << "Online ID  Found:" <<mthis->recvIP << endl;
		//printf("%s", CIPbuf);
		emit mthis->newConnect();
		
		bool isExist=false;
		for (int i = 0; i < 100; i++)
		{
			if (mthis->iplist[i] == str)  isExist = true;
		}
		if (!isExist)
		{
			mthis->iplist[mthis->ipcnt] = str;
			mthis->ipcnt++;
			//cout << mthis->iplist[0];
		}
		
		
	}
}

void send(void*p)
{
	UDPSocket*udp = (UDPSocket*)p;
	char str = '1';
	while (1)
	{
		Sleep(1000);	
		int size = sizeof(str);
		udp->sendto((char*)mthis->LanIP.c_str(), 1024);
	//	udp->sendto(&str, size);
	}
}
string* CIPFinder::ipListPtr()
{
	return iplist;
}
string CIPFinder::getMyHostIP()
{
	QString localHostName = QHostInfo::localHostName();

	//qDebug() << "localName" << localHostName;

	QHostInfo info = QHostInfo::fromName(localHostName);

	//qDebug() << "hip:" << info.addresses();

	foreach(QHostAddress address, info.addresses())   //取得區網IP
	{
		if (address.protocol() == QAbstractSocket::IPv4Protocol)
		{
		QLanIP = address.toString();
		LanIP = QLanIP.toStdString();
		}
	}

	QList<QHostAddress> list = QNetworkInterface::allAddresses();
	foreach(QHostAddress address, list)
	{
		if (address.protocol() == QAbstractSocket::IPv4Protocol)
			;
			//我们使用IPv4地址  
			//qDebug() << "gg" << address.toString();
	}

	string i;
	return i;
}
void CIPFinder::processWhatMyIPReply(QNetworkReply *reply) //處理whatsmyip回傳資料
{
	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray ipAddress = reply->readAll();
		QString str1(ipAddress);
		QString str2 = str1.mid(2519, 2520);
		QExternalIP=str2.section(QString("<"), 0, 0);
		ExternalIP = QExternalIP.toStdString();
		//cout << ExternalIP;
	}
	else
	{
		QMessageBox::information(this, "Network error", "Could not get your external IP address. Your interwebs seem to be down.");
	}
	reply->deleteLater();
}
CIPFinder::CIPFinder(bool sendMyIP)
{
	mthis = this;

	getMyHostIP();

	iplist[0] == "null";

	UDPSocket *udp;
	udp = new UDPSocket("225.6.7.8", 36598);

	if (sendMyIP)	
	_beginthread(send, 0, udp);
	else
	_beginthread(recv, 0, udp);
}
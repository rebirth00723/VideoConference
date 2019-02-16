#include"CSockChecker.h"
CSockChecker* mthis;
char* buf = (char*)malloc(100);
void f(void*p)
{
	UDPSocket*udp = (UDPSocket*)p;

	while (1)
	{
		udp->recvfrom(buf, 100);
		mthis->recvdId = *buf - '0';
		cout << "Online ID  Found:" << mthis->recvdId << endl;


		sprintf(mthis->recvdVip, "225.6.7.8:%s111", buf);
		cout << "Online VideoIP Found:" << mthis->recvdVip << endl;

		sprintf(mthis->recvdAip, "225.6.7.8:%s222", buf);
		cout << "Online AudioIP Found:" << mthis->recvdAip << endl;

		sprintf(mthis->recvdSip, "225.6.7.8:%s555", buf);
		cout << "Online ScreenIP Found:" << mthis->recvdSip << endl;
		emit mthis->newConnect();
	}
}

void sendid(void*p)
{
	UDPSocket*udp = (UDPSocket*)p;
	while (1)
	{
		Sleep(1000);
		char* ip = (char*)malloc(100);
		sprintf(ip, "%d", mthis->myid);
		udp->sendto(ip, 1024);
	}
}

CSockChecker::CSockChecker(int id)
{
	mthis = this;
	myid = id;


	recvdVip = (char*)malloc(100);
	recvdAip = (char*)malloc(100);
	recvdSip = (char*)malloc(100);
	int recvdId;
	char* ip;

	udList.clear();

	UDPSocket *udp;
	udp = new UDPSocket("225.6.7.8", 12345);

	_beginthread(f, 0, udp);
	_beginthread(sendid, 0, udp);
}
std::map<int, CUserData>* CSockChecker::getMemList()
{
	return &udList;
}
void CSockChecker::setMemList(map<int, CUserData> udlist)
{
	this->udList = udlist;
}
bool CSockChecker::hasUser(int uid)
{
	auto it = udList.find(uid);

	if (it == udList.end())
		return false;
	return true;
}
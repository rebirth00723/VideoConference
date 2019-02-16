#include <QtWidgets/QApplication>
#include <stdlib.h>
#include <iostream>
#include"CSockChecker.h" //important  dont change
#include"../VideoConference/CMediaPlayer.h"     //important dont change
#include"../VideoConference/ConferenceUI.h"//important  dont change
#include"../LIbScreenShare/H264Codec.h"
#include"../LibScreenShare/ImageUtils.h"
#include"../LibVideoConference/CNetPainter.h"
//#include"../LibScreenShare/RTPService.h"  //includes windows.h(winsock) so must be added last
#include"../LibVideoConference/Client.h"
#include"../LibVideoConference/Server.h"
#include <windows.h>
#include"QtZlib\zlib.h"
#include"WorkerThread.h"


using namespace std;
#define w 640
#define h 480
#define wi 1920
#define hi 1080

#define MaxBufferSize 999999
static CMediaSender MediaSender;
static CMediaSender dMediaSender;
static CMediaSender AudioSender;

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

	CIPFinder find(0);
	Sleep(4000);

	SockClient			client("127.0.0.1:356");
	guiLogin			*guilogin;
	guiClient			*guiclient;
	std::string ip;


	WorkerThread *wt = new WorkerThread(nullptr);
	wt->ms->show();
	QCoreApplication::processEvents();
	
	int connectCnt = 0;
	bool connected = false;
	do{
		Sleep(1000);

		//ip = "127.0.0.1:356";
		ip = find.iplist[0] + ":356"; 
		cout <<"ff" <<find.iplist[0]; cout <<"hh"<< find.LanIP;
		if (ip.length() > 8 && ip.length()<20&&find.iplist[0]!=find.LanIP)
			
		{
			cout << "TryTry";
			  if (client.connect(ip.c_str()))
			  {
				  cout << "connect" << ip.c_str();
				  wt->ms->hide();
				  QCoreApplication::processEvents();
				  connected = true;
				  break;
			  }
		}

	  connectCnt++;

	} while (connectCnt<5);


	
	if (!connected) {
	wt->ms->hide();
	wt->timeoutms->show();
	QCoreApplication::processEvents();
		do{
			
			
			Sleep(1000);
			cout << "\nConnect To Host";
			if (client.connect("127.0.0.1:356"))
			{
				wt->timeoutms->hide();
				break;
			}

		}while(1);
	}
	

	guilogin = new guiLogin(&AudioSender, &MediaSender, &dMediaSender);
	guilogin->setGuiClient(&guiclient);
	guilogin->setSock(&client);
	guilogin->show();


	CPacketer pack;
	CodeBehavior code;
	std::string send;
	std::string recv;
	CMeetingData md;
	CUserData_arr uds;
	CMeetingData_arr mds;
	int_arr uids;
	int_arr vts;
	int_arr times;
	std::set<int> _set;
	std::map<int, std::set<int>>		mv_map; 
	int time;
	int deadLine;
	int _mid;
	int uid;
	uids = std::make_unique<int[]>(2);
	vts = std::make_unique<int[]>(2);
	md.admin = 1;
	md.content = "1";
	md.deadLine = 2;
	md.title = "2";
	md.memAmount = 2;
	uids[0] = 1;
	uids[1] = 2;
	vts[0] = 1;
	vts[1] = 2;

	code = C_CREATE_MEETING;

	send =
		pack.Init().
		insert(&code).
		insert(&md).
		insert(uids.get(), 2).
		insert(vts.get(), 2).
		getData();

	int mid = 36;
	int count;
	uid = 1;
	code = C_REQUEST_USER_MEETING;
	//send = pack.Init().insert(&code).insert(&uid).getData();
	//recv = client.sendCommand(send);
	pack.Init(recv).pull(&code);

	if (code > 0)
	{
		pack.pull(&mds, &count);

		while (pack.hasData())
		{
			_set.clear();
			pack.pull(&_mid).
				pull(&deadLine).
				pull(&times, &count);
			for (int i = 0; i < count; i++)
				_set.insert(times[i]);
			mv_map[_mid] = _set;
		}
	}
	_sleep(3000);


	return a.exec();

}

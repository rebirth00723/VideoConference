#include <QtWidgets/QApplication>
#include <stdlib.h>
#include <iostream>
#include "qbuffer.h"
#include "qtcpsocket.h"
#include "FileUI.h"
#include <process.h>

#include "../LibVideoConference/CMediaPlayer.h"
#include "../LibVideoConference/CVideoRecorder.h"
#include "../LibVideoConference/MyWindow.h"
#include "../LibVideoConference/ClientDialogString.h"
#include"../LibVideoConference/ClientDialog.h"
#include"../LibVideoConference/ConferenceUI.h"
#include"../LibVideoConference/login.h"
#include"../LibVideoConference/mainwindow.h"
#include"../LibVideoConference/meetingInformationDialog.h"
#include"../LibVideoConference/operatingInformationDialog.h"
#include"../LibVideoConference/setPassword.h"
#include"../LibVideoConference/Dialog.h"
#include"../LibVideoConference/Dialogforbti.h"
#include"../LibVideoConference/ServerGUI1.h"
#include"../LibVideoConference/CNetPainter.h"

#include "../LibScreenShare/ScreenCapturer.h"
#include "../LibScreenShare/CameraCapturer.h"
#include "../LIbScreenShare/H264Codec.h"
#include "../LibScreenShare/ImageUtils.h"
#include "../LibScreenShare/SQLService.h"
#include "../LibScreenShare/RTPService.h"  //includes windows.h(winsock) so must be added last

using namespace std;
#define w 640
#define h 480
#define w 1920
#define h 1080
#define max 50
#pragma comment(lib,"../Debug/LibVideoConference.lib")
//char* buf = (char*)malloc(1024);
//char* ip;
bool s = false;
ConferenceUI* mainUI;

/*
void f( void*p)
{
	UDPSocket*udp = (UDPSocket*)p;
	while (1)
	{
		udp->recvfrom(buf, 1024);
		sprintf(buf, "225.6.7.8:%s", ip);
		cout << buf;
		mainUI->addMember(buf, "225.6.7.8:4444");
	}
}
*/


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);	
	
	/*int myid = 3;

	char* audioip = (char*)malloc(1000);
	sprintf(audioip, "225.6.7.8:100%d", myid);

	char* videoip = (char*)malloc(1000);
	sprintf(videoip, "225.6.7.8:200%d", myid);

	char* screenip = (char*)malloc(1000);
	sprintf(screenip, "225.6.7.8:300%d", myid);

	CMediaSender AudioSender;
	AudioSender.setupProperties(DISABLE, audioip);
	AudioSender.start();
	
	CMediaSender MediaSender;
	MediaSender.setupProperties(videoip, DISABLE, w, h, VQ_NORMAL, AQ_HIGH, true);
	MediaSender.start();

	CMediaSender dMediaSender;
	dMediaSender.setupProperties(screenip, DISABLE, w, h, VQ_NORMAL, AQ_HIGH, false);
	dMediaSender.start();*/
	
	login login;
	//login.show();
	mainwindow m;
	m.show();
	meetingInformationDialog MID;
	operatingInformationDialog OID;
	setPassword sp;
	//sp.show();


	/*mainUI = new ConferenceUI();
	mainUI->closeAll();

	mainUI->setmyid(myid);

	int memberlist[] = { 3, 2, 0, 100 };
	mainUI->setMemberlist(memberlist);
	QObject::connect(&MID, &meetingInformationDialog::enterConference, [=](){

		mainUI->show();
		mainUI->closeAll();
		mainUI->addByIdlist();
		mainUI->resize(1024, 568);

	});*/

	QObject::connect(&login, SIGNAL(login_Success()), &m, SLOT(show_Mainwindow()));

	QObject::connect(&m, SIGNAL(show_meetingInformation()), &MID, SLOT(show_information()));

	QObject::connect(&m, SIGNAL(show_operatingInformation()), &OID, SLOT(show_operatingInformation()));

	QObject::connect(&m, SIGNAL(signout_Success()), &login, SLOT(show_login()));

	QObject::connect(&m, SIGNAL(show_setPassword()), &sp, SLOT(show_setPassword()));


	MeetingData nsmd[max];
	nsmd[0].meetid = 0000;
	nsmd[0].meetname = "這是待開會議";
	nsmd[0].starttime = "2017/12/12 12:12";
	nsmd[1].meetid = 0001;
	nsmd[1].meetname = "123";
	nsmd[1].starttime = "2017/11/11 11:11";
	nsmd[2].meetid = 0002;
	nsmd[2].meetname = "123000";
	nsmd[2].starttime = "2017/12/31 23:59";
	for (int i = 0; i < 3; i++)
	{
		m.getNotStartedMeetingData(nsmd[i], i);
		m.searchCount1++;
	}
	m.tab1_page1_scrollArea->setWidget(m.tab1_page1_scrollAreaWidget);


	MeetingData ndmd[max];
	ndmd[0].meetid = 0003;
	ndmd[0].meetname = "這是未定會議";
	ndmd[1].meetid = 0004;
	ndmd[1].meetname = "1212";
	ndmd[2].meetid = 0005;
	ndmd[2].meetname = "0001313";
	for (int i = 0; i < 3; i++)
	{
		m.getNotDecidedMeetingData(ndmd[i], i);
		m.searchCount2++;
	}
	m.tab1_page2_scrollArea->setWidget(m.tab1_page2_scrollAreaWidget);


	MeetingData nrmd[max];
	nrmd[0].meetid = 0006;
	nrmd[0].meetname = "這是待審會議1";
	nrmd[0].voteingDeadline = "2017/10/10 10:00";
	nrmd[0].creater = "白金之星";
	nrmd[0].issue = "123456789";
	nrmd[0].votingOptions = "2017/11/11 10:28,2017/11/11 10:29,2017/11/11 10:30,2017/11/11 10:31";
	nrmd[1].meetid = 0006;
	nrmd[1].meetname = "這是待審會議2";
	nrmd[1].voteingDeadline = "2017/12/12 10:00";
	nrmd[1].creater = "專題之星";
	nrmd[1].issue = "8+9+17";
	nrmd[1].votingOptions = "2017/11/11 23:59,2017/11/12 01:30";
	for (int i = 0; i < 2; i++)
	{
		m.getNotReviewedMeetingData(nrmd[i], i);
	}

	MeetingData hmd[max];
	hmd[0].meetid = 0007;
	hmd[0].meetname = "這是歷史會議1";
	hmd[0].starttime = "2016/10/04 10:00";
	hmd[0].endtime = "2016/10/04 12:00";
	hmd[0].creater = "專題之星";
	hmd[0].staff = "參加者1,參加者2,參加者3";
	hmd[0].issue = "這是會議說明1";
	hmd[1].meetid = 0007;
	hmd[1].meetname = "這是歷史會議2";
	hmd[1].starttime = "2017/05/04 16:30";
	hmd[1].endtime = "2017/05/04 18:30";
	hmd[1].creater = "專題之星";
	hmd[1].staff = "參加者1";
	hmd[1].issue = "這是會議說明2";
	hmd[2].meetid = 0007;
	hmd[2].meetname = "這是歷史會議3";
	hmd[2].starttime = "2017/08/08 19:20";
	hmd[2].endtime = "2017/08/08 20:00";
	hmd[2].creater = "專題之星";
	hmd[2].staff = "參加者1,參加者2";
	hmd[2].issue = "這是會議說明3";
	for (int i = 0; i < 3; i++)
	{
		m.getHistoricalMeetingData(hmd[i], i);
	}


	/*ServerGUI1 ww;
	Dialog x;
	Dialogforbti y;
	//ww.show();
	QObject::connect(&ww, SIGNAL(showuserdata()), &x, SLOT(showud()));
	QObject::connect(&ww, SIGNAL(data1()), &y, SLOT(showud()));*/

	//CNetPainter painter;
	//painter.show();
    return a.exec();
}

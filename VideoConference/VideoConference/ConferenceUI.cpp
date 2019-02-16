#include"ConferenceUI.h"
#include"../VideoConference/CMediaPlayer.h"
#include"../LibVideoConference/MyWindow.h"

QTimer timer1;
int idlist[100] = {0};
int watchingDesk = 0;
void ConferenceUI::newCon()
{
	if (sock->recvdId > 0)
	{
	if (!idlist[sock->recvdId])
	{
		this->addMember(sock->recvdVip, sock->recvdAip, sock->recvdSip,sock->recvdId);
		idlist[sock->recvdId] = 1;
	}
	}
	//this->addByIdlist();       // NOT GOOD
}

ConferenceUI::ConferenceUI(int id)
{
	QPalette kk;
	kk.setColor(QPalette::Window, QColor(213,214,255));
	
	this->setPalette(kk);
	myid = id;
	playerCnt = 0;
	chat = new MyWindow("225.6.7.8",7878);

	chat->setID(myid);               //TBD
	
	sock = new CSockChecker(myid);

	closeBtn = new QPushButton(qs("離開會議"), this);
	fullBtn = new QPushButton(qs("放大畫面"), this);
	closeBtn->setFixedSize(70, 50);   //TBD
	QObject::connect(closeBtn, SIGNAL(clicked()), this, SLOT(closeBtn_clicked()));
	QObject::connect(fullBtn, SIGNAL(clicked()), this, SLOT(fullBtn_clicked()));
	fd = new FDialog;
	fd->setFixedSize(130, 200);         //TBD
	cfe = new CFileExplorer;
	cfe->setFixedSize(100, 200);        //TBD
	////////////////////////////////
	mainlayout = new QVBoxLayout(this);
	layout = new QHBoxLayout();

	toplayout = new QHBoxLayout();
	desklayout = new QHBoxLayout();

	botlayout = new QHBoxLayout();
	Llayout = new QHBoxLayout();
	melayout = new QHBoxLayout();
	Rlayout = new QHBoxLayout();
	/////////////////////////////////
	
	mainlayout->addLayout(toplayout);
	mainlayout->addLayout(layout);
	mainlayout->addLayout(botlayout);

	toplayout->addLayout(desklayout);
	toplayout->addWidget(chat);

	botlayout->addLayout(Llayout);
	botlayout->addLayout(melayout);
	botlayout->addLayout(Rlayout);

	Llayout->addWidget(closeBtn);
	//Llayout->addWidget(fd);
	//Llayout->addWidget(cfe);


	painter = new CNetPainter();

	painter->setFixedSize(300,150);         //TBD
	Rlayout->addWidget(painter);



	this->setMinimumSize(640, 480);
	
	QObject::connect(&timer1, &QTimer::timeout, [=](){ this->update(); newCon(); });
	timer1.start(50);

	chat->setFixedSize(200, 300);              //TBD
	
	/*for full screen desk view;*////
	fl = new QHBoxLayout(&qw);    
	QObject::connect(&qw, &CDeskView::exit, [=](){ 	fl->removeWidget(deskplayers[watchingDesk]->nvs);
	desklayout->addWidget(deskplayers[watchingDesk]->nvs);
	qw.hide(); });
	/*for select channel*/
	csc = new CSelectChannel(idlist, sock->getMemList());
	QObject::connect(csc, SIGNAL(ok()), this, SLOT(showBychecklist()));

	selectChannelBtn = new QPushButton(qs("切換桌面實況頻道"), this);
	selectChannelBtn->setFixedSize(300, 100);
	Llayout->addWidget(selectChannelBtn);

	QObject::connect(selectChannelBtn, SIGNAL(clicked()), this, SLOT(selectChannelBtn_clicked()));
}
void ConferenceUI::setMid(int mid)
{
	this->mid = mid;
}
void ConferenceUI::setMyid(int id){
	myid = id;
}
void ConferenceUI::addplayer(CStreamPlayer* widget, bool isScreenShare) //CVideoSurface CStreamPlayer
{
	this->setMinimumSize(0, 0);

	if (isScreenShare){ 
		desklayout->addWidget(widget->nvs); 
		
		return; 
	}  ////////////////////////////////
	if (widget->memberID == myid)
	{
		melayout->addWidget(widget->nvs);
		widget->nvs->setFixedSize(240, 240);             //TBD
	}
		

	else
	{
		layout->addWidget(widget->nvs);
		widget->nvs->setFixedSize(280, 250);           //TBD
	}
}
void ConferenceUI::addMember(char* vip, char* aip, char* sip, int id)
{
	players[playerCnt] = new CStreamPlayer();
	
	if (id == myid)players[playerCnt]->setAddr(vip);
	else 
		players[playerCnt]->setAddr(vip, aip);
	players[playerCnt]->memberID = id;               //must before this->addplayer(players[playerCnt], false); !!!!!!!
	this->addplayer(players[playerCnt], false);

	
	deskplayers[playerCnt] = new CStreamPlayer();     ////////////////////////////////////
	deskplayers[playerCnt]->setAddr(sip);
	this->addplayer(deskplayers[playerCnt], true);
	deskplayers[playerCnt]->memberID = id;
	
	playerCnt++;
	closeAll();
}


void ConferenceUI::closeBtn_clicked()
{
	this->hide();
	closeAll();

	emit exitConference(mid);   //傳送離開會議訊號
}

void ConferenceUI::selectChannelBtn_clicked()
{
	csc->updateList();
	csc->show();
	//cout << "gg" << csc->memberlistExPtr[2];
}
bool ConferenceUI::isSameMeeting(int mid)
{
	return this->mid == mid;
}
void ConferenceUI::setUserDataList(map<int, CUserData> udlist)
{
	sock->setMemList(udlist);
}
void ConferenceUI::fullBtn_clicked()
{
	fl->addWidget(deskplayers[watchingDesk]->nvs);
	qw.showFullScreen();	
}
void ConferenceUI::addByIdlist()
{

	for (int k = 0; k < playerCnt; k++)
	{
		if (sock->hasUser(players[k]->memberID)){
			players[k]->nvs->show();
			players[k]->closed = false;

			
			deskplayers[k]->nvs->show();            ///////////////////////////
			deskplayers[k]->closed = false;
			
		}
	}
}
void ConferenceUI::showBychecklist()    //根據勾選項目顯示桌面實況
{

	for (int k = 0; k < playerCnt; k++) //關閉所有桌面實況
	{
		deskplayers[k]->nvs->hide();           
		deskplayers[k]->closed = true;
	}

	for (int k = 0; k < playerCnt; k++)
	{
		if (csc->checklist[deskplayers[k]->memberID] == 1)
		{
			deskplayers[k]->nvs->show();
			deskplayers[k]->closed = false;
			watchingDesk = k;
		}

	}
}
void ConferenceUI::closeAll()
{
	for (int k = 0; k < playerCnt; k++)
	{
		players[k]->nvs->hide();
		players[k]->closed = true;
	
		deskplayers[k]->nvs->hide();           //////////////////////////
		deskplayers[k]->closed = true;
	}
}
void ConferenceUI::closeEvent(QCloseEvent * event)
{
	closeAll();
}

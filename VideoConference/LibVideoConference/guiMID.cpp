#include "guiMID.h"
using namespace std;
guiMID::guiMID(int uid, SockClient *sock,  QWidget *parent)
	: QDialog(parent)
{
	this->client = sock;
	this->uid = uid;
	colordefine.setEDMIDColor(&blackBG, &orangeText, &btnStyleSheet);
	//search mem
	cui = new ConferenceUI(uid);
	blackBase.setColor(QPalette::Base, Qt::black);

	this->setWindowTitle(qs("會議資訊"));
	this->setFixedWidth(400);
	this->setFont(QFont("", 10, QFont::Black));
	this->setPalette(blackBG);

	QObject::connect(cui, SIGNAL(exitConference(int)), this, SLOT(leaveConference(int)));
	information_Widget();
	information_Layout();
}
void guiMID::showMID(CMeetingData md)
{
	int i = md.id;
	this->mid = i;
	
	CPacketer pack;
	CodeBehavior code;
	std::string send;
	std::string recv;
	std::string _staff = "";

	member_data.release();
	udList.clear();

	code = C_REQUEST_MEETING_MEM_DATA;
	send = pack.Init().insert(&code).insert(&i).getData();
	recv = client->sendCommand(send);
	pack.Init(recv).pull(&code).pull(&member_data, &count);

	enter->hide();
	leave->show();
	name->setText(QString::fromStdString(md.title.c_str()));
	issue->setText(QString::fromStdString(md.content.c_str()));
	creater->setText(QString::fromStdString((*UIDData)[md.admin].name.c_str()));

	for (int _i = 0; _i < count; _i++)
	{
		udList[member_data[_i].id] = member_data[_i];
		CUserData _ud = member_data[_i];
		_staff += _ud.name + ", ";
	}

	staff->setText(QString::fromStdString(_staff.c_str()));

	QDateTime dateTime = QDateTime::currentDateTime();
	int cTime = dateTime.toTime_t();

	if (md.start == 0)
	{
		date->setText(qs("尚未決定，投票表決中..."));
	}	
	else
	{
		date->setText(timeStampToQString(md.start));

		if (cTime >= md.start)
		{
			leave->hide();
			enter->show();
		}
	}
	this->exec();
}
void guiMID::information_Widget()
{
	name_t = new QLabel(qs("會議名稱:"));
	name_t-> setPalette(orangeText);
	name_t->setFixedWidth(55);
	date_t = new QLabel(qs("會議時間:"));
	date_t->setPalette(orangeText);
	date_t->setFixedWidth(55);
	issue_t = new QLabel(qs(" 會議說明"));
	issue_t->setPalette(orangeText);
	issue_t->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
	issue_t->setFixedWidth(55);
	creater_t = new QLabel(qs("建立者:"));
	creater_t->setPalette(orangeText);
	creater_t->setAlignment(Qt::AlignTop);
	creater_t->setFixedWidth(55);
	staff_t = new QLabel(qs("參與者:"));
	staff_t->setPalette(orangeText);
	staff_t->setAlignment(Qt::AlignTop);
	staff_t->setFixedWidth(55);
	name = new QLabel();
	name->setPalette(orangeText);
	date = new QLabel();
	date->setPalette(orangeText);
	issue = new QTextBrowser();
	issue->setPalette(blackBase);
	issue->setTextColor(QColor(205, 102, 0));
	issue->setFrameStyle(QFrame::NoFrame);
	issue->setMaximumHeight(100);
	creater = new QLabel();
	creater->setPalette(orangeText);
	creater->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	staff = new QLabel();
	staff->setPalette(orangeText);
	staff->adjustSize();
	staff->setWordWrap(true);
	staff->setAlignment(Qt::AlignLeft | Qt::AlignTop);

	enter = new QPushButton(qs("進入會議"));
	enter->setStyleSheet(btnStyleSheet);
	enter->setFixedHeight(30);
	enter->setFont(QFont("", 12, QFont::Black));
	connect(enter, SIGNAL(clicked()), this, SLOT(enter_clicked()));

	leave = new QPushButton(qs("離開"));
	leave->setStyleSheet(btnStyleSheet);
	leave->setFixedHeight(30);
	leave->setFont(QFont("", 12, QFont::Black));
	connect(leave, SIGNAL(clicked()), this, SLOT(close()));
}
void guiMID::information_Layout()
{
	hbox1 = new QHBoxLayout();
	hbox2 = new QHBoxLayout();
	hbox3 = new QHBoxLayout();
	hbox4 = new QHBoxLayout();
	hbox5 = new QHBoxLayout();

	hbox1->addWidget(name_t);
	hbox1->addWidget(name);
	hbox2->addWidget(date_t);
	hbox2->addWidget(date);
	hbox3->addWidget(issue_t);
	hbox3->addWidget(issue);
	hbox4->addWidget(creater_t);
	hbox4->addWidget(creater);
	hbox5->addWidget(staff_t);
	hbox5->addWidget(staff);

	vbox1 = new QVBoxLayout(this);
	vbox1->setSpacing(10);
	vbox1->setContentsMargins(30, 20, 30, 20);
	vbox1->addLayout(hbox1);
	vbox1->addLayout(hbox2);
	vbox1->addLayout(hbox3);
	vbox1->addLayout(hbox4);
	vbox1->addLayout(hbox5);
	vbox1->addWidget(enter);
	vbox1->addWidget(leave);
}
QString guiMID::timeStampToQString(int timeStamp)
{
	QDateTime	time;
	QString		deadline;

	time = QDateTime::fromTime_t(timeStamp);
	deadline = time.toString("yyyy/MM/dd hh:mm");
	return deadline;
}
void guiMID::getMapdata(std::map<int, CUserData> * data2)
{
	UIDData = data2;
}

void guiMID::leaveConference(int mid)
{
	emit setMeeting(0);
}
void guiMID::enter_clicked()
{
	
	QFile logFile("log.txt");
	logFile.open(QIODevice::WriteOnly);
	logFile.write("Clicked");
	logFile.close();

	cui->setMid(mid);

	cui->setUserDataList(udList);
	cui->show();
	cui->closeAll();
	cui->addByIdlist();
	emit setMeeting(mid);
	this->hide();
}


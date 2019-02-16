#include "guiClient.h"
using namespace std;
#define w 640
#define h 480
#define wi 1920
#define hi 1080

guiClient::~guiClient()
{
}
guiClient::guiClient(int uid, SockClient *client, CMediaSender* A, CMediaSender* D, CMediaSender *M,  QWidget *parent)
	: QWidget(parent)
{
	colordefine.setClientColor(&blackBackground, &blackBase, &orangeWT, &orangeBase, &orangeText,
								&greenStatus, &redStatus, &btnStyleSheet, &tabStyle, &tabWidget);
	
	std::string __uid = std::to_string(uid);
	//M->setupIPPORT("225.6.7.8:"+__uid+"111", DISABLE);
	//D->setupIPPORT("225.6.7.8:" + __uid + "555", DISABLE);
	//A->setupIPPORT(DISABLE, "225.6.7.8:"+__uid+"222");

	D->setupProperties("225.6.7.8:" + __uid + "555", DISABLE, w, h, VQ_NORMAL, AQ_HIGH, false);
	M->setupProperties("225.6.7.8:" + __uid + "111", DISABLE, w, h, VQ_NORMAL, AQ_HIGH, true);
	A->setupProperties(DISABLE, "225.6.7.8:" + __uid + "222");

	M->start();
	A->start();
	D->start();
	//AudioSender->start();
	//dMediaSender->start();



	this->client = client;
	this->setWindowTitle(qs("會議主頁"));
	this->setFixedSize(830, 640);
	this->setFont(QFont("", 10, QFont::Black));
	ui_frame = new QFrame();
	ui_frame->setFrameStyle(QFrame::Box | QFrame::Sunken);

	ui_Widget(); 
	ui_Layout();

	/*Tabwidget1*/
	tabwidget1 = new QTabWidget();
	tabwidget1->setFixedHeight(415);
	tab1_page1 = new QWidget();
	tabwidget1->addTab(tab1_page1, qs("待開會議"));
	tab1_page2 = new QWidget();
	tabwidget1->addTab(tab1_page2, qs("未定會議"));
	tab1_page3 = new QWidget();
	tabwidget1->addTab(tab1_page3, qs("搜尋會議"));
	tab1_Widget();
	tab1_TotalLayout();

	/*Tabwidget2*/
	tabwidget2 = new QTabWidget();
	tabwidget2->setFixedWidth(410);
	tab2_page1 = new QWidget();
	tab2_page2 = new QWidget();
	tab2_page3 = new QWidget();
	
	
	tab2_page1_Widget();
	tab2_page1_Layout();
	tab2_page2_Widget();
	tab2_page2_Layout();
	tab2_page3_Widget();
	tab2_page3_Layout();
	/*--------------------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------總介面----------------------------------------------------*/
	/*--------------------------------------------------------------------------------------------------------------*/
	leftLayouot = new QVBoxLayout();
	leftLayouot->addWidget(ui_frame);
	leftLayouot->addWidget(tabwidget1);

	allLayout = new QHBoxLayout(this);
	allLayout->setSpacing(10);
	allLayout->setContentsMargins(15,10,15,10);
	allLayout->addLayout(leftLayouot);
	allLayout->addWidget(tabwidget2);
	setGuiColor();
	/*--------------------------------------------------------------------------------------------------------------*/
	/*----------------------------------------------------初始化----------------------------------------------------*/
	/*--------------------------------------------------------------------------------------------------------------*/
	init_data(uid);
	/*--------------------------------------------------------------------------------------------------------------*/
	/*---------------------------------------------------事件觸發---------------------------------------------------*/
	/*--------------------------------------------------------------------------------------------------------------*/
	/*ui:時鐘*/
	connect(ui_timer, SIGNAL(timeout()), this, SLOT(displayTime()));
	/*ui:登出*/
	connect(ui_signout, SIGNAL(clicked(bool)), this, SLOT(dobtn_tab2()));
	/*ui:設定*/
	connect(ui_setPassword, SIGNAL(clicked(bool)), this, SLOT(dobtn_tab2()));
	/*ui:使用資訊*/
	connect(ui_useInformation, SIGNAL(clicked(bool)), this, SLOT(dobtn_tab2()));
	/*搜尋會議*/
	connect(tab1_page3_search, SIGNAL(textChanged(QString)), this, SLOT(tab1_Searching()));
	/*待審會議:查看會議資訊*/
	connect(tab2_page1_tablewidget, SIGNAL(cellClicked(int, int)), this, SLOT(viewMeetingData(const int, const  int)));
	/*待審會議:時間選擇*/
	connect(tab2_page1_add_btn, SIGNAL(clicked(bool)), this, SLOT(dobtn_tab2()));
	connect(tab2_page1_cancel_btn, SIGNAL(clicked(bool)), this, SLOT(dobtn_tab2()));
	connect(tab2_page1_date_list1, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(chooseData(QListWidgetItem *)));
	connect(tab2_page1_date_list2, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(chooseData(QListWidgetItem *)));
	/*待審會議:送出*/
	connect(tab2_page1_sentout_btn, SIGNAL(clicked(bool)), this, SLOT(dobtn_tab2()));

	/*創建會議:選擇群組*/
	connect(tab2_page2_department, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateGroupsList(const QString &)));
	//重製connect(tabwidget2, SIGNAL(currentChanged(int)), this, SLOT(resetCreateTab()));
	/*創建會議:顏色設定*/
	connect(tab2_page2_issue, SIGNAL(textChanged()), this, SLOT(setEditingColor()));
	
	/*創建會議:新增刪除檔案事件*/
	connect(tab2_page2_addfile_btn, SIGNAL(clicked(bool)), this, SLOT(dobtn_tab2()));
	/*創建會議:新增刪除時間事件*/
	connect(tab2_page2_add_btn, SIGNAL(clicked(bool)), this, SLOT(dobtn_tab2()));
	connect(tab2_page2_delete_btn, SIGNAL(clicked(bool)), this, SLOT(dobtn_tab2()));
	/*創建會議:新增刪除人員事件*/
	connect(tab2_page2_addall_btn, SIGNAL(clicked(bool)), this, SLOT(dobtn_tab2()));
	connect(tab2_page2_deleteall_btn, SIGNAL(clicked(bool)), this, SLOT(dobtn_tab2()));
	connect(tab2_page2_list1, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(chooseData(QListWidgetItem *)));
	connect(tab2_page2_list2, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(chooseData(QListWidgetItem *)));
	/*創建會議:創建*/
	connect(tab2_page2_create_btn, SIGNAL(clicked(bool)), this, SLOT(dobtn_tab2()));
	/*歷史會議:重置*/
	connect(tab2_page3_search, SIGNAL(textChanged(QString)), this, SLOT(searchingMode()));
	/*歷史會議:搜尋會議*/
	connect(tab2_page3_searchbtn, SIGNAL(clicked(bool)), this, SLOT(dobtn_tab2()));
	/*歷史會議:查看會議資訊*/
	connect(tab2_page3_tablewidget, SIGNAL(cellClicked(int, int)), this, SLOT(viewMeetingData(const int, const  int)));


	QObject::connect(this, SIGNAL(show_operatingInformation()), &guiOID, SLOT(show_operatingInformation()));
	QObject::connect(this, SIGNAL(show_setPassword()), &guiSP, SLOT(show_setPassword()));
	QObject::connect(this, SIGNAL(showMID(CMeetingData)), guimID, SLOT(showMID(CMeetingData)));

	QObject::connect(&guiED, SIGNAL(signout_Success()), this, SLOT(lognout_Success()));
	QObject::connect(&CPackage, SIGNAL(showMID(int)), this, SLOT(showMID_(int)));
	QObject::connect(guimID, SIGNAL(setMeeting(int)), this, SLOT(setMeeting(int)));

	QObject::connect(&timer1, SIGNAL(timeout()), this, SLOT(samd()));

}
void guiClient::init_data(int uid)
{
	myid = uid;

	currentOnMeeting = 0;
	currentChooseGroup = 0;
	udata = { 0, "", "", 0, 0, 0, 0, 0};
	gd_map.clear();
	ud_map.clear();
	gm_map.clear();
	member.clear();

	if (uds != nullptr)
		uds.release();
	if (gds != nullptr)
		gds.release();

	showTargetGroupsList();
	showMemberList();

	tab2_page2_name->clear();
	tab2_page2_issue->clear();
	tab2_page2_datelist->clear();
	tab2_page2_list1->clear();
	tab2_page2_list2->clear();
	tab2_page2_date->setDate(QDate::currentDate());
	tab2_page2_time->setTime(QTime::currentTime());
	tab2_page2_datetime->setDateTime(QDateTime::currentDateTime());

	/////////sql
	int count;

	CPacketer pack;
	CodeBehavior code;
	std::string send;
	std::string recv;

	code = C_REQUEST_USER_DATA;
	send = pack.Init().insert(&code).insert(&uid).getData();
	recv = client->sendCommand(send);
	pack.Init(recv).pull(&code).pull(&udata);

	code = C_REQUEST_GROUPS;
	send = pack.Init().insert(&code).getData();
	recv = client->sendCommand(send);
	pack.Init(recv).pull(&code).pull(&gds, &count);

	for (int i = 0; i < count; i++)
		gd_map[gds[i].id] = gds[i].name;
	//sql end
	updateUsersData();
	refreshGroupMem();
	if (udata.isCreator)
	{
		tabwidget2->addTab(tab2_page1, qs("         待審會議          "));
		tabwidget2->addTab(tab2_page2, qs("         創建會議         "));
		tabwidget2->addTab(tab2_page3, qs("         歷史會議         "));
	
		updateGroupCombo();
		showTargetGroupsList();
	}
	else
	{
		tabwidget2->addTab(tab2_page1, qs("               待審會議                "));
		tabwidget2->addTab(tab2_page3, qs("               歷史會議                "));
	}


	timer1.start(5000);
	qShowAllMeetin = new QThread();
	timer1.moveToThread(qShowAllMeetin);
	qShowAllMeetin->start();

	guimID = new guiMID(uid, client);
	guimID->getMapdata(&(this->ud_map));
	
	inputUserData(udata);

}
void guiClient::ui_Widget()
{
	ui_nameTitle = new QLabel(qs("使用者名稱 :"));
	ui_nameTitle->setFont(QFont("", 14, QFont::Bold));
	ui_name = new QLabel;
	ui_idTitle = new QLabel(qs("UID :"));
	ui_idTitle->setFont(QFont("", 14, QFont::Bold));
	ui_id = new QLabel;
	ui_id->setFixedWidth(150);
	ui_departmentTitle = new QLabel(qs("部門單位 :"));
	ui_departmentTitle->setFont(QFont("", 14, QFont::Bold));
	ui_department = new QLabel;
	ui_signout = new QPushButton(qs("   登出   "));
	ui_setPassword = new QPushButton(qs(" 更改密碼 "));
	ui_useInformation = new QPushButton(qs(" 使用資訊 "));
	ui_timeLCD = new QLCDNumber();
	ui_timeLCD->setDigitCount(16);
	ui_timeLCD->setMode(QLCDNumber::Dec);
	ui_timeLCD->setSegmentStyle(QLCDNumber::Flat);
	ui_timeLCD->setFrameStyle(QFrame::NoFrame);
	ui_timer = new QTimer();
	ui_timer->start(1000);
}
void guiClient::ui_Layout()
{
	ui_gbox = new QGridLayout();
	ui_gbox->addWidget(ui_nameTitle, 0, 0, 1, 1);
	ui_gbox->addWidget(ui_name, 0, 1, 1, 3);
	ui_gbox->addWidget(ui_idTitle, 1, 0, 1, 1);
	ui_gbox->addWidget(ui_id, 1, 1, 1, 3);
	ui_gbox->addWidget(ui_departmentTitle, 2, 0, 1, 1);
	ui_gbox->addWidget(ui_department, 2, 1, 1, 3);

	ui_btn_vbox = new QVBoxLayout();
	ui_btn_vbox->addWidget(ui_signout);
	ui_btn_vbox->addWidget(ui_setPassword);
	ui_btn_vbox->addWidget(ui_useInformation);

	ui_hbox = new QHBoxLayout();
	ui_hbox->addLayout(ui_gbox);
	ui_hbox->addLayout(ui_btn_vbox);

	ui_vbox = new QVBoxLayout(ui_frame);
	ui_vbox->addLayout(ui_hbox);
	ui_vbox->addWidget(ui_timeLCD);
}
void guiClient::tab1_Widget()
{
	tab1_page1_scrollAreaWidget = new QWidget();
	tab1_page1_formLayout = new QFormLayout(tab1_page1_scrollAreaWidget);
	tab1_page1_scrollArea = new QScrollArea();
	tab1_page1_scrollArea->setFrameShape(QFrame::NoFrame);
	tab1_page1_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tab1_page1_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	tab1_page2_scrollAreaWidget = new QWidget();
	tab1_page2_formLayout = new QFormLayout(tab1_page2_scrollAreaWidget);
	tab1_page2_scrollArea = new QScrollArea();
	tab1_page2_scrollArea->setFrameShape(QFrame::NoFrame);
	tab1_page2_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tab1_page2_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	tab1_page3_searchtitle = new QLabel(qs("請輸入會議名稱"));
	tab1_page3_search = new QLineEdit();

	tab1_page3_hbox1 = new QHBoxLayout();
	tab1_page3_hbox1->addWidget(tab1_page3_searchtitle);
	tab1_page3_hbox1->addWidget(tab1_page3_search);
	tab1_page3_scrollAreaWidget = new QWidget();
	tab1_page3_formLayout = new QFormLayout(tab1_page3_scrollAreaWidget);
	tab1_page3_scrollArea = new QScrollArea();
	tab1_page3_scrollArea->setFrameShape(QFrame::NoFrame);
	tab1_page3_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	tab1_page3_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}
void guiClient::tab1_TotalLayout()
{
	tab1_page1_vbox = new QVBoxLayout(tab1_page1);
	tab1_page1_vbox->addWidget(tab1_page1_scrollArea);

	tab1_page2_vbox = new QVBoxLayout(tab1_page2);
	tab1_page2_vbox->addWidget(tab1_page2_scrollArea);

	tab1_page3_vbox = new QVBoxLayout(tab1_page3);
	tab1_page3_vbox->addLayout(tab1_page3_hbox1);
	tab1_page3_vbox->addWidget(tab1_page3_scrollArea);
}
void guiClient::tab2_page1_Widget()
{
	tab2_page1_tablewidget = new QTableWidget();
	tab2_page1_tablewidget->verticalHeader()->hide();
	tab2_page1_tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);				//設定選擇行為，以行為單位
	tab2_page1_tablewidget->setSelectionMode(QAbstractItemView::SingleSelection);				//設定選擇模式，單行
	tab2_page1_tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);					//設定為不可編輯
	tab2_page1_tablewidget->setFixedHeight(200);
	tab2_page1_tablewidget->setColumnCount(6);
	QStringList headers;
	headers << qs("會議名稱") << qs("投票期限") << qs("建立者");
	tab2_page1_tablewidget->setHorizontalHeaderLabels(headers);
	tab2_page1_tablewidget->setColumnWidth(0, 180);
	tab2_page1_tablewidget->setColumnWidth(1, 120);
	tab2_page1_tablewidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
	tab2_page1_tablewidget->hideColumn(3);
	tab2_page1_tablewidget->hideColumn(4);
	tab2_page1_tablewidget->hideColumn(5);
	//////////////////////////////////////////////////////////////////////////////////////////////
	tab2_page1_information = new QWidget();
	tab2_page1_name = new QLabel(qs("會議名稱 : "));
	tab2_page1_name->setFont(QFont("", 10, QFont::Black));
	tab2_page1_issue = new QLabel(qs("會議說明 (會議議題 & 會議議程) :"));
	tab2_page1_issue->setFont(QFont("", 10, QFont::Black));
	tab2_page1_issue_TB = new QTextBrowser();
	tab2_page1_issue_TB->setFont(QFont("", 10, QFont::Black));
	tab2_page1_issue_TB->setTextColor(QColor(205, 102, 0));
	tab2_page1_issue_TB->setFrameStyle(QFrame::NoFrame);
	tab2_page1_time = new QLabel(qs("請選擇可出席時間:"));
	tab2_page1_time->setFont(QFont("", 10, QFont::Black));
	tab2_page1_date_list1 = new QListWidget();
	tab2_page1_date_list2 = new QListWidget();
	tab2_page1_add_btn = new QPushButton(qs("加入"));
	tab2_page1_add_btn->setFixedHeight(25);
	tab2_page1_cancel_btn = new QPushButton(qs("取消"));
	tab2_page1_cancel_btn->setFixedHeight(25);
	tab2_page1_file = new QLabel(qs("附加檔案:"));
	tab2_page1_file->setFont(QFont("", 10, QFont::Black));
	tab2_page1_file->setAlignment(Qt::AlignTop);
	tab2_page1_fileList = new QListWidget();
	tab2_page1_fileList->setFixedHeight(40);
	tab2_page1_fileList->setFrameStyle(QFrame::NoFrame);
	tab2_page1_fileDownload = new QPushButton(qs("下載檔案"));
	tab2_page1_fileDownload->setFixedHeight(30);
	tab2_page1_sentout_btn = new QPushButton(qs("送出"));
	tab2_page1_sentout_btn->setFixedHeight(30);
}
void guiClient::tab2_page1_Layout()
{
	tab2_page1_btn_vbox = new QVBoxLayout();
	tab2_page1_btn_vbox->addWidget(tab2_page1_add_btn);
	tab2_page1_btn_vbox->addWidget(tab2_page1_cancel_btn);

	tab2_page1_choosedate_hbox = new QHBoxLayout();
	tab2_page1_choosedate_hbox->addWidget(tab2_page1_date_list1);
	tab2_page1_choosedate_hbox->addLayout(tab2_page1_btn_vbox);
	tab2_page1_choosedate_hbox->addWidget(tab2_page1_date_list2);

	tab2_page1_file_hbox = new QHBoxLayout();
	//tab2_page1_file_hbox->addWidget(tab2_page1_file);
	//tab2_page1_file_hbox->addWidget(tab2_page1_fileList);
	//tab2_page1_file_hbox->addWidget(tab2_page1_fileDownload);


	tab2_page1_information_gbox = new QGridLayout(tab2_page1_information);
	tab2_page1_information_gbox->addWidget(tab2_page1_name, 0, 0, 1, 1);
	tab2_page1_information_gbox->addWidget(tab2_page1_issue, 1, 0, 1, 1);
	tab2_page1_information_gbox->addWidget(tab2_page1_issue_TB, 2, 0, 1, 1);
	tab2_page1_information_gbox->addWidget(tab2_page1_time, 3, 0, 1, 1);
	tab2_page1_information_gbox->addLayout(tab2_page1_choosedate_hbox, 4, 0, 1, 1);
	tab2_page1_information_gbox->addLayout(tab2_page1_file_hbox, 5, 0, 1, 1);
	tab2_page1_information_gbox->addWidget(tab2_page1_sentout_btn, 6, 0, 1, 1);
	//////////////////////////////////////////////////////////////////////////////////////////////
	tab2_page1_vbox = new QVBoxLayout(tab2_page1);
	tab2_page1_vbox->setSpacing(6);
	tab2_page1_vbox->addWidget(tab2_page1_tablewidget);
	hSpacer = new QSpacerItem(0, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);
	tab2_page1_vbox->addItem(hSpacer);
	tab2_page1_vbox->addWidget(tab2_page1_information);
}
void guiClient::tab2_page2_Widget()
{
	tab2_page2_nametitle = new QLabel();
	tab2_page2_nametitle->setText(qs("會議名稱 "));
	tab2_page2_name = new QLineEdit();
	tab2_page2_issuetitle = new QLabel();
	tab2_page2_issuetitle->setText(qs("會議說明:(會議議題 & 會議議程)"));
	tab2_page2_addfile_btn = new QPushButton();
	tab2_page2_addfile_btn->setText(qs("附加檔案"));
	tab2_page2_addfile_btn->setFixedHeight(30);
	tab2_page2_issue = new QTextEdit();
	tab2_page2_issue->setFixedHeight(80);
	tab2_page2_issue->setTextColor(QColor(205, 102, 0));
	tab2_page2_fileList = new QListWidget();
	tab2_page2_fileList->setFixedHeight(30);
	tab2_page2_fileList->setFrameStyle(QFrame::NoFrame);
	/*-------------------------------------------------------------------------*/
	tab2_page2_timetitle = new QLabel();
	tab2_page2_timetitle->setText(qs("會議時間"));
	tab2_page2_date = new QDateTimeEdit(QDate::currentDate());
	//tab2_page2_date->setCalendarPopup(true);
	tab2_page2_time = new QDateTimeEdit(QTime::currentTime());
	tab2_page2_add_btn = new QPushButton();
	tab2_page2_add_btn->setFixedHeight(20);
	tab2_page2_add_btn->setText(qs("新增"));
	tab2_page2_delete_btn = new QPushButton();
	tab2_page2_delete_btn->setFixedHeight(20);
	tab2_page2_delete_btn->setText(qs("刪除"));
	tab2_page2_datelist = new QListWidget();
	tab2_page2_votetitle = new QLabel();
	tab2_page2_votetitle->setText(qs("投票期限"));
	tab2_page2_datetime = new QDateTimeEdit();
	tab2_page2_datetime->setDateTime(QDateTime::currentDateTime());
	//tab2_page2_datetime->setCalendarPopup(true);
	/*-------------------------------------------------------------------------*/
	tab2_page2_stafftitle = new QLabel();
	tab2_page2_stafftitle->setText(qs("參與人員"));
	tab2_page2_department = new QComboBox();
	tab2_page2_department->addItem(qs("全部"));
	tab2_page2_list1 = new QListWidget();
	tab2_page2_list2 = new QListWidget();
	tab2_page2_addall_btn = new QPushButton();
	tab2_page2_addall_btn->setText(qs("全部加入"));
	tab2_page2_addall_btn->setFixedHeight(20);
	tab2_page2_deleteall_btn = new QPushButton();
	tab2_page2_deleteall_btn->setText(qs("全部移除"));
	tab2_page2_deleteall_btn->setFixedHeight(20);
}
void guiClient::tab2_page2_Layout()
{
	tab2_page2_name_hbox = new QHBoxLayout();
	tab2_page2_name_hbox->addWidget(tab2_page2_nametitle);
	tab2_page2_name_hbox->addWidget(tab2_page2_name);
	tab2_page2_file_hbox = new QHBoxLayout();
//	tab2_page2_file_hbox->addWidget(tab2_page2_fileList);
//	tab2_page2_file_hbox->addWidget(tab2_page2_addfile_btn);

	tab2_page2_program_vbox = new QVBoxLayout();
	tab2_page2_program_vbox->setSpacing(6);
	tab2_page2_program_vbox->addLayout(tab2_page2_name_hbox);
	tab2_page2_program_vbox->addWidget(tab2_page2_issuetitle);
	tab2_page2_program_vbox->addWidget(tab2_page2_issue);
	tab2_page2_program_vbox->addLayout(tab2_page2_file_hbox);
	/*-------------------------------------------------------------------------*/
	tab2_page2_timebutton_hbox = new QHBoxLayout();
	tab2_page2_timebutton_hbox->addWidget(tab2_page2_add_btn);
	tab2_page2_timebutton_hbox->addWidget(tab2_page2_delete_btn);

	tab2_page2_datetime_vbox = new QVBoxLayout();
	tab2_page2_datetime_vbox->addWidget(tab2_page2_timetitle);
	tab2_page2_datetime_vbox->addWidget(tab2_page2_date);
	tab2_page2_datetime_vbox->addWidget(tab2_page2_time);
	tab2_page2_datetime_vbox->addLayout(tab2_page2_timebutton_hbox);

	tab2_page2_timevote_hbox = new QHBoxLayout();
	tab2_page2_timevote_hbox->addWidget(tab2_page2_votetitle);
	tab2_page2_timevote_hbox->addWidget(tab2_page2_datetime);

	tab2_page2_time_vbox = new QVBoxLayout();
	tab2_page2_time_vbox->addWidget(tab2_page2_datelist);
	tab2_page2_time_vbox->addLayout(tab2_page2_timevote_hbox);

	tab2_page2_time_hbox = new QHBoxLayout();
	tab2_page2_time_hbox->addLayout(tab2_page2_datetime_vbox);
	tab2_page2_time_hbox->addLayout(tab2_page2_time_vbox);
	/*-------------------------------------------------------------------------*/
	tab2_page2_list1_vbox = new QVBoxLayout();
	tab2_page2_list1_vbox->addWidget(tab2_page2_stafftitle);
	tab2_page2_list1_vbox->addWidget(tab2_page2_department);
	tab2_page2_list1_vbox->addWidget(tab2_page2_list1);

	tab2_page2_list2_vbox = new QVBoxLayout();
	hSpacer = new QSpacerItem(0, 50, QSizePolicy::Minimum, QSizePolicy::Fixed);
	tab2_page2_list2_vbox->addItem(hSpacer);
	tab2_page2_list2_vbox->addWidget(tab2_page2_list2);

	tab2_page2_listbtn_vbox = new QVBoxLayout();
	hSpacer = new QSpacerItem(0, 220, QSizePolicy::Minimum, QSizePolicy::Fixed);
	tab2_page2_listbtn_vbox->addItem(hSpacer);
	tab2_page2_listbtn_vbox->addWidget(tab2_page2_addall_btn);
	tab2_page2_listbtn_vbox->addWidget(tab2_page2_deleteall_btn);

	tab2_page2_hbox = new QHBoxLayout();
	tab2_page2_hbox->addLayout(tab2_page2_list1_vbox);
	tab2_page2_hbox->addLayout(tab2_page2_listbtn_vbox);
	tab2_page2_hbox->addLayout(tab2_page2_list2_vbox);
	/////////////////////////////////////////////////////////////////////////////
	tab2_page2_vbox = new QVBoxLayout(tab2_page2);
	tab2_page2_vbox->setContentsMargins(15, 10, 15, 10);
	tab2_page2_vbox->setSpacing(6);
	tab2_page2_vbox->addLayout(tab2_page2_program_vbox);
	tab2_page2_vbox->addLayout(tab2_page2_time_hbox);
	tab2_page2_vbox->addLayout(tab2_page2_hbox);

	tab2_page2_create_btn = new QPushButton();
	tab2_page2_create_btn->setText(qs("創建"));
	tab2_page2_create_btn->setFixedHeight(30);
	tab2_page2_vbox->addWidget(tab2_page2_create_btn);
}
void guiClient::tab2_page3_Widget()
{
	tab2_page3_searchTitle = new QLabel(qs("搜尋:"));
	tab2_page3_search = new QLineEdit();
	tab2_page3_searchcombo = new QComboBox();
	tab2_page3_searchcombo->insertItem(0, qs("全部"));
	tab2_page3_searchcombo->insertItem(1, qs("會議名稱"));
	tab2_page3_searchcombo->insertItem(2, qs("會議時間"));
	tab2_page3_searchcombo->insertItem(3, qs("創立者"));
	tab2_page3_searchbtn = new QPushButton(qs(" 搜尋 "));

	tab2_page3_tablewidget = new QTableWidget();
	tab2_page3_tablewidget->verticalHeader()->hide();
	tab2_page3_tablewidget->setSelectionBehavior(QAbstractItemView::SelectRows);				//設定選擇行為，以行為單位
	tab2_page3_tablewidget->setSelectionMode(QAbstractItemView::SingleSelection);				//設定選擇模式，單行
	tab2_page3_tablewidget->setEditTriggers(QAbstractItemView::NoEditTriggers);				//設定為不可編輯
	tab2_page3_tablewidget->setFixedHeight(200);
	tab2_page3_tablewidget->setColumnCount(6);
	QStringList headers;
	headers << qs("會議名稱") << qs("會議時間") << qs("建立者");
	tab2_page3_tablewidget->setHorizontalHeaderLabels(headers);
	tab2_page3_tablewidget->setColumnWidth(0, 180);
	tab2_page3_tablewidget->setColumnWidth(1, 120);
	tab2_page3_tablewidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
	tab2_page3_tablewidget->hideColumn(3);
	tab2_page3_tablewidget->hideColumn(4);
	tab2_page3_tablewidget->hideColumn(5);
	/*------------------------------------------------------------------------------------------*/
	tab2_page3_nameTitle = new QLabel(qs("會議名稱 :"));
	tab2_page3_name = new QLabel();
	tab2_page3_startTimeTitle = new QLabel(qs("開始時間:"));
	tab2_page3_startTime = new QLabel();
	tab2_page3_endTimeTitle = new QLabel(qs("結束時間:"));
	tab2_page3_endTime = new QLabel();
	tab2_page3_issueTitle = new QLabel(qs("會議說明:"));
	tab2_page3_issue = new QTextBrowser;
	tab2_page3_issue->setTextColor(QColor(205, 102, 0));
	tab2_page3_issue->setFixedHeight(100);
	tab2_page3_issue->setFrameStyle(QFrame::NoFrame);
	tab2_page3_staffTitle = new QLabel(qs("參與人員:"));
	tab2_page3_staff = new QTextBrowser;
	tab2_page3_staff->setTextColor(QColor(205, 102, 0));
	tab2_page3_staff->setFixedHeight(50);
	tab2_page3_staff->setFrameStyle(QFrame::NoFrame);
	tab2_page3_fileTitle = new QLabel(qs("附加檔案 :"));
	tab2_page3_fileList = new QListWidget();
	tab2_page3_fileList->setFrameStyle(QFrame::NoFrame);
	tab2_page3_fileList->setFixedHeight(70);
	tab2_page3_fileDownload = new QPushButton(qs("下載檔案"));
	tab2_page3_fileDownload->setFixedHeight(70);
}
void guiClient::tab2_page3_Layout()
{
	tab2_page3_search_hbox = new QHBoxLayout();
	tab2_page3_search_hbox->addWidget(tab2_page3_searchTitle);
	tab2_page3_search_hbox->addWidget(tab2_page3_search);
	tab2_page3_search_hbox->addWidget(tab2_page3_searchcombo);
	tab2_page3_search_hbox->addWidget(tab2_page3_searchbtn);

	tab2_page3_file_hbox = new QHBoxLayout();
//	tab2_page3_file_hbox->addWidget(tab2_page3_fileList);
//	tab2_page3_file_hbox->addWidget(tab2_page3_fileDownload);

	tab2_page3_gbox = new QGridLayout();
	tab2_page3_gbox->setContentsMargins(10, 0, 0, 0);
	tab2_page3_gbox->addWidget(tab2_page3_nameTitle, 0, 0, 1, 1);
	tab2_page3_gbox->addWidget(tab2_page3_name, 0, 1, 1, 5, Qt::AlignLeft);
	tab2_page3_gbox->addWidget(tab2_page3_startTimeTitle, 1, 0, 1, 1);
	tab2_page3_gbox->addWidget(tab2_page3_startTime, 1, 1, 1, 5, Qt::AlignLeft);
	tab2_page3_gbox->addWidget(tab2_page3_endTimeTitle, 2, 0, 1, 1);
	tab2_page3_gbox->addWidget(tab2_page3_endTime, 2, 1, 1, 5, Qt::AlignLeft);
	tab2_page3_gbox->addWidget(tab2_page3_issueTitle, 3, 0, 1, 1, Qt::AlignTop);
	tab2_page3_gbox->addWidget(tab2_page3_issue, 3, 1, 1, 3);
	tab2_page3_gbox->addWidget(tab2_page3_staffTitle, 4, 0, 1, 1, Qt::AlignTop);
	tab2_page3_gbox->addWidget(tab2_page3_staff, 4, 1, 1, 3);
	//tab2_page3_gbox->addWidget(tab2_page3_fileTitle, 5, 0, 1, 1, Qt::AlignTop);
	tab2_page3_gbox->addLayout(tab2_page3_file_hbox, 5, 1, 1, 1);

	tab2_page3_vbox = new QVBoxLayout(tab2_page3);
	tab2_page3_vbox->setContentsMargins(11, 15, 11, 15);
	tab2_page3_vbox->setSpacing(6);
	tab2_page3_vbox->addLayout(tab2_page3_search_hbox);
	tab2_page3_vbox->addWidget(tab2_page3_tablewidget);
	hSpacer = new QSpacerItem(0, 5, QSizePolicy::Minimum, QSizePolicy::Fixed);
	tab2_page3_vbox->addItem(hSpacer);
	tab2_page3_vbox->addLayout(tab2_page3_gbox);

}

int guiClient::timeStampConversion(QDateTime timeT)
{
	return timeT.toTime_t();
}
char guiClient::QStringToChar(QString str)
{
	string s = str.toStdString();
	const char* c = s.c_str();
	return *c;
}
QDateTime guiClient::QStringToDateTime(QString timeOption)
{
	QDateTime time;
	time = QDateTime::fromString(timeOption, "yyyy/MM/dd hh:mm");
	return time;
}
QString guiClient::timeStampToQString(int timeStamp)
{
	QDateTime	time;
	QString		deadline;

	time = QDateTime::fromTime_t(timeStamp);
	deadline = time.toString("yyyy/MM/dd hh:mm");
	return deadline;
}
QString guiClient::votingOptionToQString(int *a)
{
	QString option;
	QStringList optionList;

	int size = _msize(a) / 4;
	for (int i = 0; i < size; i++)
	{
		//qDebug() << a[i];
		optionList.append(timeStampToQString(a[i]));
	}
	option = optionList.join(",");
	//qDebug() << option;

	return option;
}
QString guiClient::intToQString(int idint)
{
	QString idqstring = QString::number(idint);
	return idqstring;
}
QString guiClient::charToQString(char* c)
{
	QByteArray byte;
	byte = QByteArray(c);
	QString str;
	str = QString::fromUtf8(byte);
	return str;
}

void guiClient::setGuiColor()
{
	this->setPalette(blackBackground);
	ui_frame->setPalette(orangeWT);

	tabwidget1->setStyleSheet(tabStyle);
	tab1_page1->setStyleSheet(tabWidget);
	tab1_page2->setStyleSheet(tabWidget);
	tab1_page3->setStyleSheet(tabWidget);

	tabwidget2->setStyleSheet(tabStyle);
	tab2_page1->setStyleSheet(tabWidget);
	tab2_page2->setStyleSheet(tabWidget);
	tab2_page3->setStyleSheet(tabWidget);

	tab2_page3_tablewidget->horizontalHeader()->setStyleSheet("color: #CD6600");

	ui_signout->setStyleSheet(btnStyleSheet);
	ui_setPassword->setStyleSheet(btnStyleSheet);
	ui_useInformation->setStyleSheet(btnStyleSheet);
	tab2_page1_add_btn->setStyleSheet(btnStyleSheet);
	tab2_page1_cancel_btn->setStyleSheet(btnStyleSheet);
	tab2_page1_sentout_btn->setStyleSheet(btnStyleSheet);
	tab2_page1_fileDownload->setStyleSheet(btnStyleSheet);
	tab2_page2_add_btn->setStyleSheet(btnStyleSheet);
	tab2_page2_delete_btn->setStyleSheet(btnStyleSheet);
	tab2_page2_addall_btn->setStyleSheet(btnStyleSheet);
	tab2_page2_deleteall_btn->setStyleSheet(btnStyleSheet);
	tab2_page2_create_btn->setStyleSheet(btnStyleSheet);
	tab2_page2_addfile_btn->setStyleSheet(btnStyleSheet);
	tab2_page3_searchbtn->setStyleSheet(btnStyleSheet);
	tab2_page3_fileDownload->setStyleSheet(btnStyleSheet);
}
void guiClient::setMeeting(int id)
{
	CPacketer pack;
	std::string send;
	CodeBehavior code;
	auto it = md_map.find(currentOnMeeting);
	if (it != md_map.end())
	if (!id && myid == it->second.admin)
	{
		code = C_END_MEETING;
		send = 
		pack.Init().
			insert(&code).
			insert(&currentOnMeeting).
			getData();
		client->sendCommand(send);
		samd();
	}
		
	currentOnMeeting = id;

}
void guiClient::lognout_Success()
{
	emit show_login();

	while (tabwidget2->count()!=0)
	{
		tabwidget2->removeTab(0);
}
}
void guiClient::displayTime()
{
	QDateTime dateTime = QDateTime::currentDateTime();
	ui_timeLCD->display(dateTime.toString("yyyy-MM-dd HH:mm"));
}
void guiClient::enterMeetingBtn()
{
	emit show_meetingInformation();
}
void guiClient::viewMeetingData(const int row, const  int col)
{
	QTableWidget* a = qobject_cast<QTableWidget*>(sender());
	if (a == tab2_page1_tablewidget)
	{
		tab2_page1_name->clear();
		tab2_page1_issue_TB->clear();
		while (tab2_page1_date_list1->count() != 0)
			tab2_page1_date_list1->takeItem(0);
		while (tab2_page1_date_list2->count() != 0)
			tab2_page1_date_list2->takeItem(0);
		
		
		QString name = a->item(row, 0)->text();
		
		currentSelectMeetingId = a->item(row, 5)->text().toInt();
		QString str0 = qs("會議名稱 : ");
		name = str0.append(name);
		tab2_page1_name->setText(name);

		QString issue = a->item(row, 3)->text();
		tab2_page1_issue_TB->setText(issue);

		QString data = a->item(row, 4)->text();
		QStringList stringList = data.split(",", QString::SkipEmptyParts);
		tab2_page1_date_list1->addItems(stringList);
		for (int i = 0; i < tab2_page1_date_list1->count(); i++)
		{
			tab2_page1_date_list1->item(i)->setForeground(QColor(205, 102, 0));
		}
	}
	if (a == tab2_page3_tablewidget)
	{
		tab2_page3_name->clear();
		tab2_page3_startTime->clear();
		tab2_page3_endTime->clear();
		tab2_page3_issue->clear();
		tab2_page3_staff->clear();

		QString name = a->item(row, 0)->text();
		tab2_page3_name->setText(name);

		QString startTime = a->item(row, 1)->text();
		tab2_page3_startTime->setText(startTime);

		QString endTime = a->item(row, 4)->text();
		tab2_page3_endTime->setText(endTime);

		QString issue = a->item(row, 3)->text();
		tab2_page3_issue->setText(issue);

		QString mem = a->item(row, 5)->text();
		tab2_page3_staff->setText(mem);
	}
}
void guiClient::chooseData(QListWidgetItem *it)
{
	QListWidget* a = qobject_cast<QListWidget*>(sender());
	QString index_s;
	int		index;
	int uid;

	/*待審會議*/
	if (a == tab2_page1_date_list1){
		it = a->takeItem(a->currentRow());
		tab2_page1_date_list2->addItem(it);
	}else
	if (a == tab2_page1_date_list2){
		it = a->takeItem(a->currentRow());
		tab2_page1_date_list1->addItem(it);
	}else
	/*創建會議*/
	if (a == tab2_page2_list1){

		index_s = a->currentItem()->text();
		uid = index_s.split("(").at(1).split(")").at(0).toInt();

		auto _it = ud_map.find(uid);
		if (_it != ud_map.end())
		{
			int gid = _it->second.groupid;
			auto __it = gm_map.find(gid);
			if (__it != gm_map.end())
			{
				std::set<int> *vec =&(__it->second);
				vec->erase(uid);
				member.insert(uid);
			}
		}
		it = a->takeItem(a->currentRow());
		showMemberList();
	}
	if (a == tab2_page2_list2){
		index_s = a->currentItem()->text();
		uid = index_s.split("(").at(1).split(")").at(0).toInt();

		auto _it = ud_map.find(uid);
		if (_it != ud_map.end())
		{
			CUserData _ud = _it->second;
			gm_map[_ud.groupid].insert(uid);
			member.erase(uid);
		}
		it = a->takeItem(a->currentRow());
		showTargetGroupsList();
	}
}
void guiClient::tab1_Searching()
{
	
	QString text = tab1_page3_search->text();										//紀錄lineedit當前字串

	if (text.contains(lastSearchText) || lastSearchText.contains(text))				//字串相同或包含就清除目前搜尋到的資料
	{
		while (tab1_page3_formLayout->count() != 0)
		{
			QLayoutItem *forDeletion = tab1_page3_formLayout->takeAt(0);
			delete forDeletion->widget();
			delete forDeletion;
		}
	}

	if (lastSearchText != text)		//字串沒變則不搜尋
	{
		int i = 0;
		for (auto it : md_map)
		{
			CMeetingData _md = it.second;
			QString str1 = QString::fromStdString((_md.title).c_str());
			QString str2 = timeStampToQString(_md.start);
			if (str1.contains(text) && !text.isEmpty())
			{
				tab1_page3_frame[i] = new QFrame;
				if (_md.start == 0)
					CPackage.newFrame(tab1_page3_frame[i], _md.id, str1, str2, 1);
				else	
					CPackage.newFrame(tab1_page3_frame[i], _md.id, str1, str2, 0);
				tab1_page3_formLayout->setWidget(i, QFormLayout::LabelRole, tab1_page3_frame[i]);
			}
			i++;
		}
		
		lastSearchText = text;		//把目前字串存起來
	}
	tab1_page3_scrollArea->setWidget(tab1_page3_scrollAreaWidget);
}
void guiClient::resetCreateTab()
{
	tab2_page2_name->clear();
	tab2_page2_issue->clear();
	tab2_page2_datelist->clear();
	tab2_page2_list1->clear();
	tab2_page2_list2->clear();
	tab2_page2_date->setDate(QDate::currentDate());
	tab2_page2_time->setTime(QTime::currentTime());
	tab2_page2_datetime->setDateTime(QDateTime::currentDateTime());

	/*測資*/
	QStringList data;
	data << qs("孫英娥") << qs("巫亭潔") << qs("陳仁豪") << qs("錢鴻邦")
		<< qs("黃士倫") << qs("黃欣瑜") << qs("唐予琳") << qs("陳宛琪") << qs("許意婷");
	

	tab2_page2_list1->addItems(data);

	for (int i = 0; i < tab2_page2_list1->count(); i++)
		tab2_page2_list1->item(i)->setForeground(QColor(205, 102, 0));

}
void guiClient::updateGroupsList(const QString &text)
{
	if (!tab2_page2_department->currentIndex())
		currentChooseGroup = 0;
	else
		currentChooseGroup = text.split("(").at(1).split(")").at(0).toInt();

	showTargetGroupsList();
}
void guiClient::searchingMode()
{
	if (tab2_page3_search->text().isEmpty())
	{
		for (int i = 0; i < tab2_page3_tablewidget->rowCount(); i++)
		{
			tab2_page3_tablewidget->showRow(i);
		}
	}

}
void guiClient::setEditingColor()
{
	tab2_page2_issue->setTextColor(QColor(205, 102, 0));
}
int guiClient::timeStampTransfrom(int a, int **time,int deadline)
{
	int listCount = 0;
	QDateTime timeTt;
	int tt = 0;
	std::time_t ctime = std::time(nullptr);;

	switch (a)
	{
	case TIMEVOTE_NOTREVIEW:
		listCount = tab2_page1_date_list2->count();
		*time = new int[listCount];

		for (int i = 0; i < listCount; i++)
		{
			timeTt = QStringToDateTime(tab2_page1_date_list2->item(i)->text());
			(*time)[i] = timeStampConversion(timeTt);
			//qDebug() << (*time)[i];
		}
		break;
	case TIMEVOTE_CREATE:
		std::asctime(std::localtime(&ctime));
		if (ctime>=deadline)
			return TIMEVOTE_EORRER;

		listCount = tab2_page2_datelist->count();
		*time = new int[listCount];

		for (int i = 0; i < listCount; i++)
		{
			timeTt = QStringToDateTime(tab2_page2_datelist->item(i)->text());
			tt = timeStampConversion(timeTt);

			if (deadline >= tt)
			{
				listCount = TIMEVOTE_EORRER;
				break;
			}
				
			else
				(*time)[i] = tt;
		}
	default:
		break;
	}
	return listCount;
}
void guiClient::dobtn_tab2()
{
	QPushButton* btn = qobject_cast<QPushButton*>(sender());
	QListWidgetItem *it = new QListWidgetItem();

	if (btn == ui_signout)					//ui:登出
	{
		guiED.EventDialog(15);
	}
	if (btn == ui_setPassword)				//ui:更改密碼
	{
		emit show_setPassword();
	}
	if (btn == ui_useInformation)			//ui:使資資訊
	{
		emit show_operatingInformation();
	}
	if (btn == tab2_page1_add_btn)			//待審會議:加入
	{
		it = tab2_page1_date_list1->currentItem();
		tab2_page1_date_list1->takeItem(tab2_page1_date_list1->currentRow());
		tab2_page1_date_list2->addItem(it);
	}
	if (btn == tab2_page1_cancel_btn)		//待審會議:取消
	{
		it = tab2_page1_date_list2->currentItem();
		tab2_page1_date_list2->takeItem(tab2_page1_date_list2->currentRow());
		tab2_page1_date_list1->addItem(it);
	}
	if (btn == tab2_page1_sentout_btn)			//待審會議:送出
	{
		if (tab2_page1_date_list2->count() < 1)
		{
			guiED.EventDialog(4);
		}
		else
		{
			int *timeT1;
			int cc_ = timeStampTransfrom(TIMEVOTE_NOTREVIEW, &timeT1,0);			//已經確定的時間票 要回傳至資料庫
			int _uid = myid;
			int _mid = currentSelectMeetingId;
			int _code = C_SET_VOTED;
			CPacketer pack;
			std::string _send = pack.Init().insert(&_code).insert(&_mid).insert(&_uid).getData();
			for (int i = 0; i < cc_; i++) 
				client->sendCommand(pack.Init(_send).insert(&timeT1[i]).getData());


			tab2_page1_tablewidget->removeRow(tab2_page1_tablewidget->currentRow());
			tab2_page1_name->setText(qs("會議名稱 : "));
			tab2_page1_issue_TB->clear();
			tab2_page1_date_list1->clear();
			tab2_page1_date_list2->clear();

			delete timeT1;
			if (_code > 0)
			{
				guiED.EventDialog(5);
				samd();
			}
				
			else
				guiED.EventDialog(19);

		}
	}
	if (btn == tab2_page2_addfile_btn)		//創建會會議:附加檔案
	{
		QString s = QFileDialog::getOpenFileName(this, qs("開啟檔案"), "/", "All files(*.*)");
		if (!s.isEmpty())
		{
			s = s.section("/", -1);
			tab2_page2_addfile_btn->setEnabled(false);
		}
	}
	if (btn == tab2_page2_add_btn)					//創建會議:新增
	{

		QDateTime time1 = tab2_page2_date->dateTime();
		QDateTime time2 = tab2_page2_time->dateTime();
		QString str1 = time1.toString("yyyy/MM/dd");
		QString str2 = time2.toString(" hh:mm");
		QString str = str2.prepend(str1);

		tab2_page2_datelist->insertItem(datelist_row, str);
		tab2_page2_datelist->item(datelist_row)->setForeground(QColor(205, 102, 0));
		datelist_row++;
	}
	if (btn == tab2_page2_delete_btn)				//創建會議:刪除
	{
		if (datelist_row >= 0)
		{
			it = tab2_page2_datelist->takeItem(tab2_page2_datelist->currentRow());
			delete it;
			datelist_row--;
		}
		if (datelist_row == -1)
			datelist_row = 0;
	}
	if (btn == tab2_page2_addall_btn)				//創建會議:全部加入
	{
		if (!currentChooseGroup) //all
		{
			for (auto it = gm_map.begin(); it != gm_map.end(); ++it)
			{
				std::set<int> *notChoose = &(it->second);
				member.insert(notChoose->begin(), notChoose->end());
				notChoose->clear();
			}
			
		}
		else
		{
			auto it = gm_map.find(currentChooseGroup);
			if (it != gm_map.end())
			{
				std::set<int> *notChoose = &(it->second);
				std::set<int> temp = it->second;
				for (int _uid : temp)
				{
					notChoose->erase(_uid);
					member.insert(_uid);
				}
			}
		}
		showTargetGroupsList();
		showMemberList();
	}
	if (btn == tab2_page2_deleteall_btn)			//創建會議:全部移除
	{
		refreshGroupMem();
		member.clear();

		showTargetGroupsList();
		showMemberList();
	}
	if (btn == tab2_page2_create_btn)				//創建會議:創建
	{
		std::string _title = tab2_page2_name->text().toStdString();
		std::string _content = tab2_page2_issue->toPlainText().toStdString();
		int votingDeadline = timeStampConversion(tab2_page2_datetime->dateTime());
		/*------------------會議時間選項轉換時間戳陣列-----------------*/
		int *timeT2;
		int timegg = timeStampTransfrom(TIMEVOTE_CREATE, &timeT2, votingDeadline);


		if (tab2_page2_name->text().isEmpty())
		{
			guiED.EventDialog(6);

		}
		else if (tab2_page2_issue->toPlainText().isEmpty())
		{
			guiED.EventDialog(7);
		}
		else if (tab2_page2_datelist->count() < 1)
		{
			guiED.EventDialog(8);
		}
		else if (tab2_page2_list2->count() < 1)
		{
			guiED.EventDialog(9);
		}
		else if (timegg == TIMEVOTE_EORRER)
		{
			guiED.EventDialog(19);
		}
		else
		{
			/*在這邊把資料傳出出去
			*/
			CPacketer pack;
			CodeBehavior code;
			std::string send;
			std::string recv;

			CMeetingData md;
			int_arr uids;
			int_arr vts;
			
			int uCount;
			int vCount;

			int *_t;
			int _i;

			uCount = member.size() + 1;
			vCount = tab2_page2_datelist->count();

			md.admin = udata.id;
			md.title = _title;
			md.content = _content;
			md.deadLine = votingDeadline;
			md.memAmount = uCount;

			uids = std::make_unique<int[]>(uCount);
			vts = std::make_unique<int[]>(vCount);

			_i = 0;
			_t = uids.get();
			for (int _uid:member)
			{
				_t[_i] = _uid;
				_i++;
			}
			_t[_i] = udata.id;
			
			_t = vts.get();
			for (_i = 0; _i < vCount; ++_i)
				_t[_i] = timeT2[_i];

			code = C_CREATE_MEETING;
			send =
				pack.Init().
				insert(&code).
				insert(&udata.id).
				insert(_title.c_str(), _title.size(), true).
				insert(_content.c_str(), _content.size(), true).
				insert(&uCount).
				insert(&votingDeadline).
				insert(uids.get(), uCount, true).
				insert(vts.get(), vCount, true).
				getData();

			tab2_page2_create_btn->setEnabled(false);

			recv = client->sendCommand(send);

			tab2_page2_create_btn->setEnabled(true);
			pack.Init(recv).pull(&code);
			if (C_SUCCESS == code)
			{
				guiED.EventDialog(10);
			}
			else
			{
				int err;
				if (pack.hasData())
					pack.pull(&err);
				guiED.EventDialog(17, err);
			}
			delete timeT2;

			/*clear*/
			tab2_page2_name->clear();
			tab2_page2_issue->clear();
			tab2_page2_date->setDate(QDate::currentDate());
			tab2_page2_time->setTime(QTime::currentTime());
			tab2_page2_datetime->setDateTime(QDateTime::currentDateTime());

			tab2_page2_datelist = new QListWidget();
			while (tab2_page2_datelist->count() != 0)
				tab2_page2_datelist->takeItem(0);
			
			while (tab2_page2_list1->count() != 0)
				tab2_page2_list1->takeItem(0);

			while (tab2_page2_list2->count() != 0)
				tab2_page2_list2->takeItem(0);	

		}
	}
	if (btn == tab2_page3_searchbtn && !tab2_page3_search->text().isEmpty())			//搜尋歷史會議
	{
		int col = tab2_page3_searchcombo->currentIndex();
		QString text = tab2_page3_search->text();

		for (int row = 0; row<tab2_page3_tablewidget->rowCount(); row++)
		{
			tab2_page3_tablewidget->hideRow(row);
		}

		if (col>0)
		{
			for (int i = 0; i < tab2_page3_tablewidget->rowCount(); i++)
			{
				QTableWidgetItem *item = tab2_page3_tablewidget->item(i, col - 1);
				if (item->text().contains(text))
				{
					tab2_page3_tablewidget->showRow(i);
				}
			}
		}
		else
		{
			for (int i = 0; i<tab2_page3_tablewidget->rowCount(); i++)
			{
				for (int j = 0; j<tab2_page3_tablewidget->columnCount(); j++)
				{
					QTableWidgetItem *item = tab2_page3_tablewidget->item(i, j);
					if (item->text().contains(text))
					{
						tab2_page3_tablewidget->showRow(i);
					}
				}
			}
		}
	}
}
void guiClient::showMID_(int mid)
{

	emit showMID(getMeetingData(mid));
}
void guiClient::inputUserData(CUserData udata)
{
	std::string gpn = gd_map.find(udata.groupid)->second;
	ui_name->setText(QString::fromStdString((udata.name).c_str()));
	ui_id->setText(intToQString(udata.id));
	//ui_department->setText(intToQString(udata.groupid));
	ui_department->setText(QString::fromStdString(gpn.c_str()));
}
void guiClient::inputNotStartedMeetingData(CMeetingData data, int i)
{
	QString str1 = QString::fromStdString(data.title.c_str());
	QString str2 = timeStampToQString(data.start);

	tab1_page1_frame[i] = new QFrame;
	CPackage.newFrame(tab1_page1_frame[i], data.id, str1, str2, 0);

	tab1_page1_formLayout->setWidget(i, QFormLayout::LabelRole, tab1_page1_frame[i]);
	
}
void guiClient::inputNotDecidedMeetingData(CMeetingData data, int i)
{
	QString str = QString::fromStdString(data.title.c_str());

	tab1_page2_frame[i] = new QFrame;
	CPackage.newFrame(tab1_page2_frame[i], data.id, str, "", 1);

	tab1_page2_formLayout->setWidget(i, QFormLayout::LabelRole, tab1_page2_frame[i]);	
}
void guiClient::inputNotReviewedMeetingData(CMeetingData data, int i)
{
	tab2_page1_tablewidget->insertRow(i);
	tab2_page1_tablewidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdString((data.title).c_str())));
	tab2_page1_tablewidget->setItem(i, 1, new QTableWidgetItem(timeStampToQString(data.deadLine)));
	tab2_page1_tablewidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(ud_map[data.admin].name.c_str())));
	tab2_page1_tablewidget->setItem(i, 3, new QTableWidgetItem(QString::fromStdString((data.content).c_str())));

	auto it = mv_map.find(data.id);
	if (it != mv_map.end())
	{
		int *_vts = new int[it->second.size()];
		int _cc = 0;
		for (int _i : it->second)
		{
			_vts[_cc] = _i;
			_cc++;
		}
		tab2_page1_tablewidget->setItem(i, 4, new QTableWidgetItem(votingOptionToQString(_vts)));
		delete[]_vts;
	}
	tab2_page1_tablewidget->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(std::to_string(data.id).c_str())));
	for (int k = 0; k < 3; k++)
		tab2_page1_tablewidget->item(i, k)->setTextColor(QColor(205, 102, 0));
}
void guiClient::inputHistoricalMeetingData(CMeetingData data, int i)
{
	std::string str = "";
	
	//member_data.release();
	//udList.clear();

	tab2_page3_tablewidget->insertRow(i);
	tab2_page3_tablewidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdString((data.title).c_str())));
	tab2_page3_tablewidget->setItem(i, 1, new QTableWidgetItem(timeStampToQString(data.start)));
	tab2_page3_tablewidget->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(ud_map[data.admin].name.c_str())));
	tab2_page3_tablewidget->setItem(i, 3, new QTableWidgetItem(QString::fromStdString((data.content).c_str())));
	tab2_page3_tablewidget->setItem(i, 4, new QTableWidgetItem(timeStampToQString(data.end)));
	


	for (int k = 0; k < 3; k++)
		tab2_page3_tablewidget->item(i, k)->setTextColor(QColor(205, 102, 0));

	auto it = mm_map.find(data.id);
	if (it != mm_map.end())
	for (auto _it = it->second.begin(); _it != it->second.end(); ++_it)
	{
		str += _it->name + ",";
	}

	


	tab2_page3_tablewidget->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(str.c_str())));
}

void guiClient::showAllMeetingData()
{
	std::lock_guard<std::mutex> mLock(upd_md);

	while (tab1_page1_formLayout->count() != 0)
	{
		QLayoutItem *forDeletion = tab1_page1_formLayout->takeAt(0);
		forDeletion->widget()->close();
	}
	while (tab1_page2_formLayout->count() != 0)
	{
		QLayoutItem *forDeletion = tab1_page2_formLayout->takeAt(0);
		forDeletion->widget()->close();
		//delete forDeletion;
	}
	
	while (tab2_page1_tablewidget->rowCount() != 0)
		tab2_page1_tablewidget->removeRow(0);

	while (tab2_page3_tablewidget->rowCount() != 0)
		tab2_page3_tablewidget->removeRow(0);

	
	

	mCount0 = 0;		//紀錄有幾筆待開會議
	mCount1 = 0;		//紀錄有幾筆未定會議
	mCount2 = 0;		//紀錄有幾筆待審會議
	mCount3 = 0;		//紀錄有幾筆歷史會議

	for (auto it = md_map.begin(); it != md_map.end(); ++it)
	{
		CMeetingData md = it->second;
		int mCase = conferenceSort(&md);
		switch (mCase)
		{
		case 0:
			inputNotStartedMeetingData(md, mCount0);
			mCount0++;
			break;
		case 1://未定以投票
			inputNotDecidedMeetingData(md, mCount1);
			mCount1++;
			break;
		case 2://待審
			inputNotReviewedMeetingData(md, mCount2);
			mCount2++;
			break;
		case 3:
			inputHistoricalMeetingData(md, mCount3);
			mCount3++;
			break;
		default:
			break;
		}
	}
	tab1_page1_scrollArea->setWidget(tab1_page1_scrollAreaWidget);
	tab1_page2_scrollArea->setWidget(tab1_page2_scrollAreaWidget);	
}
int guiClient::conferenceSort(CMeetingData *data) 
{
	int conferenceCase;

	if ((data->end) == 0)
	{
		if (data->start == 0)
		{
			auto it = mv_map.find(data->id);
			if (it != mv_map.end())//有投票選項
				conferenceCase = 2;/*待審會議*/
			else
				conferenceCase = 1;/*未定會議*/
		}
		else
		{
			/*待開會議*/
			conferenceCase = 0;
		}	
	}
	else
	{
		/*歷史會議*/
		conferenceCase = 3;
	}
	return conferenceCase;
}
void guiClient::updateMeetingData()
{
	std::lock_guard<std::mutex> mLock(upd_md);

	CPacketer pack;
	CodeBehavior code;
	std::string send;
	std::string recv;
	int size1;
	int size2;
	int amount;
	int admin;
	int start;
	int end;
	int count;
	int _mid;
	int deadLine;
	int_arr times;
	char_arr title;
	char_arr content;
	std::set<int> _set;

	md_map.clear();
	mv_map.clear();

	code = C_REQUEST_USER_MEETING;
	send = pack.Init().insert(&code).insert(&myid).getData();

	recv = client->sendCommand(send);
	pack.Init(recv).pull(&code);

	if (code > 0)
	{
		pack.pull(&count);
		for (int i = 0; i < count; i++)//get all meeting
		{
			CMeetingData md;
			pack.pull(&_mid).
				pull(&title, &size1).
				pull(&content, &size2).
				pull(&amount).
				pull(&admin).
				pull(&start).
				pull(&end);

			md.id = _mid;
			md.title = std::string(title.get(), size1);
			md.content = std::string(content.get(), size2);
			md.memAmount = amount;
			md.admin = admin;
			md.start = start;
			md.end = end;

			md_map[_mid] = md;
		}

		while (pack.hasData())
		{
			_set.clear();
			pack.pull(&_mid).
				pull(&deadLine).
				pull(&times, &count);

			md_map[_mid].deadLine = deadLine;

			for (int i = 0; i < count; i++)
				_set.insert(times[i]);
			mv_map[_mid] = _set;
		}
			
	}

}
void  guiClient::updateUsersData()
{
	CPacketer pack;
	CodeBehavior code;
	std::string send;
	std::string recv;
	int size;

	code = C_REQUEST_USERS_DATA;
	send = pack.insert(&code).getData();

	//recv
	recv = client->sendCommand(send);
	pack.Init(recv).pull(&code).pull(&uds, &size);
	ud_map.clear();
	for (int i = 0; i < size; i++)
		ud_map[uds[i].id] = uds[i];
}
void guiClient::refreshGroupMem()
{
	gm_map.clear();
	for (auto _ud : ud_map)
	{
		CUserData _t = _ud.second;
		if (_t.id == udata.id)
			continue;
		gm_map[_t.groupid].insert(_t.id);
	}
}
void guiClient::updateGroupCombo()
{
	QComboBox *box = tab2_page2_department;
	const int gg = box->count();
	for (int i = gg -1; i > 0; --i)
		box->removeItem(i);

	for (auto _gd : gd_map)
	{
		std::string _str = _gd.second;
		_str = "(" + std::to_string(_gd.first) + ")" + _str;
		box->addItem(QString::fromStdString(_str.c_str()));
	}
}
void	guiClient::showTargetGroupsList()
{
	tab2_page2_list1->clear();

	if (!currentChooseGroup) //all
	{
		for (auto it = gm_map.begin(); it != gm_map.end(); ++it)
		for (auto _mem : it->second)
		{
			std::string _name;
			std::string userName;
			userName = ud_map.find(_mem)->second.name;

			_name = "(" + std::to_string(_mem) + ")" + userName;
			tab2_page2_list1->addItem(QString::fromStdString(_name.c_str()));
		}
	}
	else
	{
		auto it = gm_map.find(currentChooseGroup);
		if (it != gm_map.end())
		{
			std::set<int> mems = it->second;
			for (auto _mem : mems)
			{
				std::string _name;
				std::string userName;
				userName = ud_map.find(_mem)->second.name;

				_name = "(" + std::to_string(_mem) + ")" + userName;
				tab2_page2_list1->addItem(QString::fromStdString(_name.c_str()));
			}
		}
		else
			tab2_page2_list1->clear();
	}
}
void guiClient::showMemberList()
{
	QListWidget *memList = tab2_page2_list2;
	memList->clear();
	for (int _uid : member)
	{
		auto it = ud_map.find(_uid);
		if (it != ud_map.end())
		{
			std::string _str = it->second.name;
			_str = "(" + std::to_string(_uid) + ")" + _str;
			memList->addItem(QString::fromStdString(_str.c_str()));
		}
	}
}
 CMeetingData guiClient::getMeetingData(int mid)
 {
	 std::lock_guard<std::mutex> mLock(upd_md);
	 CMeetingData md = { 0 };
	 auto it = md_map.find(mid);
	 if (it != md_map.end())
		 return md_map[mid];
	 else
		 return  md;
}
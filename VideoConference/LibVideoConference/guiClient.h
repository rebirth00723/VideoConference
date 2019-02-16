#pragma once
#ifndef GUICLIENT_H
#define GUICLIENT_H

#include <QtWidgets/QWidget>
#include "qtablewidget.h"
#include "qtextedit.h"
#include "qdatetimeedit.h"
#include "qlistwidget.h"
#include "qdatetime.h"
#include "qcombobox.h"
#include "qcheckbox.h"
#include "qspinbox.h"
#include "qfiledialog.h"
#include "qtablewidget.h"
#include "qtextbrowser.h"
#include "qformlayout.h"
#include "qscrollarea.h"
#include "qframe.h"
#include "qheaderview.h"
#include "qlcdnumber.h"

#include "guiED.h"
#include "guiCPackage.h"
#include <../LibScreenShare/Network.h>

#include "guiOID.h"
#include "guiSP.h"
#include "guiMID.h"
#include "guiCPackage.h"

#include <iostream>
#include <map>
#include <set>

#define TIMEVOTE_CREATE				100
#define TIMEVOTE_NOTREVIEW			101
#define TIMEVOTE_EORRER				102
#define MAXITEM 50

class guiClient : public QWidget
{
	Q_OBJECT

public:
	guiClient(int uid, SockClient *client, CMediaSender*, CMediaSender*, CMediaSender*, QWidget *parent = Q_NULLPTR);
	~guiClient();
	QWidget			*tab1_page1_scrollAreaWidget;
	QScrollArea		*tab1_page1_scrollArea;
	QWidget			*tab1_page2_scrollAreaWidget;
	QScrollArea		*tab1_page2_scrollArea;
	
	int				datelist_row = 0;
	int				tablewidget_row = 0;

	int				mCount0;		//紀錄有幾筆待開會議
	int				mCount1;		//紀錄有幾筆未定會議
	int				mCount2;		//紀錄有幾筆待審會議
	int				mCount3;		//紀錄有幾筆歷史會議
	QString			lastSearchText;

	void			inputUserData(CUserData);
	void			inputNotStartedMeetingData(CMeetingData, int);
	void			inputNotDecidedMeetingData(CMeetingData, int);
	void			inputNotReviewedMeetingData(CMeetingData, int);
	void			inputHistoricalMeetingData(CMeetingData, int);
	
	int				conferenceSort(CMeetingData*);




private:
	void			showAllMeetingData();
	void			init_data(int uid);
	void			updateMeetingData();
	void			updateUsersData();
	void			updateGroupCombo();
	void			refreshGroupMem();
	void			showTargetGroupsList();
	void			showMemberList();
	CMeetingData	getMeetingData(int mid);

	std::map<int, std::string>			gd_map;
	std::map<int, CUserData>			ud_map;
	std::map<int, CMeetingData>			md_map;
	std::map<int, std::set<CUserData>>	mm_map;//meeting id, mem
	std::map<int, std::set<int>>		gm_map;
	std::map<int, std::set<int>>		mv_map;
	std::set<int>						member;

	std::mutex							upd_md;

	bool								quit;
	int					currentChooseGroup;
	int					myid;
	int					currentOnMeeting;
	int					currentSelectMeetingId;

	QThread				*qShowAllMeetin;
	QTimer				timer1;

	CMeetingData_arr	mds;
	CGroupData_arr		gds;
	CUserData_arr		uds;
	CUserData			udata;

	guiCPackage			CPackage;
	guiOID				guiOID;
	guiSP				guiSP;
	guiMID				*guimID;




	//CSQLService		sql;

	guiED			guiED;
	ColorDefine		colordefine;

	SockClient		*client;
	QPalette		blackBackground;
	QPalette		blackBase;
	QPalette		orangeWT;
	QPalette		orangeBase;
	QPalette		orangeText;
	QString			greenStatus;
	QString			redStatus;
	QString			btnStyleSheet;
	QString			tabStyle;
	QString			tabWidget;
	/*-----------------------------------------------*/
	QFrame			*ui_frame;
	QTabWidget		*tabwidget1;
	QTabWidget		*tabwidget2;

	QVBoxLayout		*leftLayouot;
	QHBoxLayout		*allLayout;
	/*-----------------------------------------------*/
	QLabel			*ui_nameTitle;
	QLabel			*ui_name;
	QLabel			*ui_idTitle;
	QLabel			*ui_id;
	QLabel			*ui_departmentTitle;
	QLabel			*ui_department;
	QLCDNumber		*ui_timeLCD;
	QTimer			*ui_timer;
	QPushButton		*ui_signout;
	QPushButton		*ui_setPassword;
	QPushButton		*ui_useInformation;

	QGridLayout		*ui_gbox;
	QHBoxLayout		*ui_hbox;
	QVBoxLayout		*ui_btn_vbox;
	QVBoxLayout		*ui_vbox;
	/*-----------------------------------------------*/
	QWidget			*tab1_page1;
	QFrame			*tab1_page1_frame[MAXITEM];

	QVBoxLayout		*tab1_page1_vbox;
	QFormLayout		*tab1_page1_formLayout;
	/*-----------------------------------------------*/
	QWidget			*tab1_page2;
	QFrame			*tab1_page2_frame[MAXITEM];

	QVBoxLayout		*tab1_page2_vbox;
	QFormLayout		*tab1_page2_formLayout;
	/*-----------------------------------------------*/
	QWidget			*tab1_page3;
	QWidget			*tab1_page3_scrollAreaWidget;
	QScrollArea		*tab1_page3_scrollArea;
	QLabel			*tab1_page3_searchtitle;
	QLineEdit		*tab1_page3_search;
	QFrame			*tab1_page3_frame[MAXITEM];
	
	QVBoxLayout		*tab1_page3_vbox;
	QFormLayout		*tab1_page3_formLayout;
	QHBoxLayout		*tab1_page3_hbox1;
	/*-----------------------------------------------*/
	QWidget			*tab2_page1;
	QWidget			*tab2_page1_information;
	QTableWidget	*tab2_page1_tablewidget;
	QLabel			*tab2_page1_name;
	QLabel			*tab2_page1_issue;
	QLabel			*tab2_page1_time;
	QLabel			*tab2_page1_file;
	QTextBrowser	*tab2_page1_issue_TB;
	QListWidget		*tab2_page1_date_list1;
	QListWidget		*tab2_page1_date_list2;
	QListWidget		*tab2_page1_fileList;
	QPushButton		*tab2_page1_add_btn;
	QPushButton		*tab2_page1_cancel_btn;
	QPushButton		*tab2_page1_sentout_btn;
	QPushButton		*tab2_page1_fileDownload;

	QVBoxLayout		*tab2_page1_vbox;
	QVBoxLayout		*tab2_page1_btn_vbox;
	QHBoxLayout		*tab2_page1_choosedate_hbox;
	QHBoxLayout		*tab2_page1_file_hbox;
	QGridLayout		*tab2_page1_information_gbox;
	/*-----------------------------------------------*/
	QWidget			*tab2_page2;
	QLabel			*tab2_page2_nametitle;
	QLabel			*tab2_page2_issuetitle;
	QLabel			*tab2_page2_timetitle;
	QLabel			*tab2_page2_stafftitle;
	QLabel			*tab2_page2_votetitle;
	QLineEdit		*tab2_page2_name;
	QTextEdit		*tab2_page2_issue;
	QPushButton		*tab2_page2_add_btn;
	QPushButton		*tab2_page2_delete_btn;
	QPushButton		*tab2_page2_addall_btn;
	QPushButton		*tab2_page2_deleteall_btn;
	QPushButton		*tab2_page2_create_btn;
	QPushButton		*tab2_page2_addfile_btn;
	QComboBox		*tab2_page2_department;
	QListWidget		*tab2_page2_datelist;
	QListWidget		*tab2_page2_list1;
	QListWidget		*tab2_page2_list2;
	QListWidget		*tab2_page2_fileList;
	QDateTimeEdit	 *tab2_page2_date;
	QDateTimeEdit	*tab2_page2_time;
	QDateTimeEdit	*tab2_page2_datetime;

	QVBoxLayout		*tab2_page2_vbox;
	QVBoxLayout		*tab2_page2_program_vbox;
	QVBoxLayout		*tab2_page2_datetime_vbox;
	QVBoxLayout		*tab2_page2_time_vbox;
	QVBoxLayout		*tab2_page2_list1_vbox;
	QVBoxLayout		*tab2_page2_list2_vbox;
	QVBoxLayout		*tab2_page2_listbtn_vbox;

	QHBoxLayout		*tab2_page2_name_hbox;
	QHBoxLayout		*tab2_page2_file_hbox;
	QHBoxLayout		*tab2_page2_timebutton_hbox;
	QHBoxLayout		*tab2_page2_time_hbox;
	QHBoxLayout		*tab2_page2_timevote_hbox;
	QHBoxLayout		*tab2_page2_hbox;
	/*-----------------------------------------------*/
	QWidget			*tab2_page3;
	QTableWidget	*tab2_page3_tablewidget;
	QListWidget		*tab2_page3_fileList;
	QLabel			*tab2_page3_searchTitle;
	QLabel			*tab2_page3_nameTitle;
	QLabel			*tab2_page3_name;
	QLabel			*tab2_page3_issueTitle;
	QLabel			*tab2_page3_startTimeTitle;
	QLabel			*tab2_page3_startTime;
	QLabel			*tab2_page3_endTimeTitle;
	QLabel			*tab2_page3_endTime;
	QLabel			*tab2_page3_staffTitle;
	QLabel			*tab2_page3_fileTitle;
	QTextBrowser	*tab2_page3_issue;
	QTextBrowser	*tab2_page3_staff;
	QLineEdit		*tab2_page3_search;
	QComboBox		*tab2_page3_searchcombo;
	QPushButton		*tab2_page3_searchbtn;
	QPushButton		*tab2_page3_fileDownload;

	QVBoxLayout		*tab2_page3_vbox;
	QHBoxLayout		*tab2_page3_search_hbox;
	QHBoxLayout		*tab2_page3_file_hbox;
	QGridLayout		*tab2_page3_gbox;
	QSpacerItem		*hSpacer;
	/*-----------------------------------------------*/
	void			ui_Widget();
	void			ui_Layout();
	void			tab1_Widget();
	void			tab1_TotalLayout();
	void			tab2_page1_Widget();
	void			tab2_page1_Layout();
	void			tab2_page2_Widget();
	void			tab2_page2_Layout();
	void			tab2_page3_Widget();
	void			tab2_page3_Layout();
	void			setGuiColor();

	int				timeStampConversion(QDateTime);		//時間戳轉換(輸出)
	char			QStringToChar(QString);
	QDateTime		QStringToDateTime(QString);	
	QString			timeStampToQString(int);			//時間戳轉換:投票期限、開始結束時間(輸入)
	QString			votingOptionToQString(int*);		//時間戳轉換:時間選項(輸入)
	QString			intToQString(int);
	QString			charToQString(char*);
	

private slots:
	void			samd()
	{
		updateMeetingData();
		showAllMeetingData();
	};
	void			setMeeting(int);
	void			lognout_Success();
	void			displayTime();			//顯示目前時間
	void			enterMeetingBtn();		//點擊會議資訊按紐
	void			viewMeetingData(const int, const int);		//查看待審會議、歷史會議資料
	void			chooseData(QListWidgetItem*);		//待審會議選時間觸發、創建會議選人的觸發
	void			tab1_Searching();		//Tab1 的搜尋
	void			resetCreateTab();		//創建會議資料重置
	void			updateGroupsList(const QString &);
	void			searchingMode();		//搜尋歷史會議事件
	void			setEditingColor();
	int				timeStampTransfrom(int select, _Out_ int **time,int);		//時間戳轉換整數陣列
	void			dobtn_tab2();	
	void			showMID_(int mid);
signals:
	void			show_login();
	void			show_setPassword();
	void			show_operatingInformation();
	void			show_meetingInformation();
	void			showMID(CMeetingData);
};
#endif
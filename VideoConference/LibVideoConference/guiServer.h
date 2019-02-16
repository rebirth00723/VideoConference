#pragma once
#ifndef GUISERVER_H
#define GUISERVER_H

#include "qtabwidget.h"
#include "qwidget.h"
#include "qtablewidget.h"
#include "qcombobox.h"
#include "qpushbutton.h"
#include "qlineedit.h"
#include "qheaderview.h"
#include "qradiobutton.h"
#include "qlabel.h"
#include "qdatetime.h"
#include "qlayout.h"
#include "qmessagebox.h"
#include "qdebug.h"
#include "SQLService.h"
#include "qthread.h"
#include "qtimer.h"
class guiServer : public QWidget
{
    Q_OBJECT

public:
	guiServer(QWidget *parent = Q_NULLPTR);

	void					inputUserData(CUserData*, int);
	void					inputGroupData(CGroupData*, int);
	//void					inputConferenceData(meetingData, int);


private:
	void					getAllData();
	void					clearAllData();

	QThread *t;
	QTimer timer;

	CSQLService				sql;

	QTabWidget				*tab;
	QWidget					*page0;
	QWidget					*page1;
	QWidget					*page2;
	QString					Register;
	
	QPushButton				*addBtn;
	/*-------------------------------------------------------------*/
	bool					addFF = false;
	QLabel					*addUserTitle;
	QLabel					*userNameTitle;
	QLineEdit				*userName;
	QLabel					*userPasswordTitle;
	QLineEdit				*userPassword;
	QLabel					*userDepartmentTitle;
	QLineEdit				*userDepartment;
	QLabel					*isCreatorTitle;
	QRadioButton			*isCreator;
	QRadioButton			*noCreator;
	QPushButton				*addUBtn;

	QHBoxLayout				*isCreatorHbox;
	QVBoxLayout				*addUserVbox;

	QLabel					*addDepartmentTitle;
	QLabel					*departmentNameTitle;
	QLineEdit				*departmentName;
	QPushButton				*addDBtn;


	void					add_Widget();
	void					add_Layout();
	void					hideAddWidget();
	void					showAddWidget();
	int						iscOrNoc();
	/*-------------------------------------------------------------*/
	QTableWidget			*page0_tableWidget;
	QLineEdit				*page0_searchLineEdit;
	QComboBox				*page0_searchComboBox;
	QPushButton				*page0_searchBtn;
	QRadioButton			*page0_allRB;
	QRadioButton			*page0_noStartedRB;			//待開會議
	QRadioButton			*page0_noDecidedRB;			//未決議會議
	QRadioButton			*page0_historicalRB;		//歷史會議
	QLabel					*page0_showStatus;

	QVBoxLayout				*page0_Vbox;
	QHBoxLayout				*page0_searchHbox;
	QHBoxLayout				*page0_rbHbox;

	void					page0_Widget();
	void					page0_Layout();
	/*-------------------------------------------------------------*/
	bool					page1_EditLock = false;
	bool					page1_updateLock = false;
	bool					page1_pwdLock = false;
	QTableWidget			*page1_tableWidget;
	QLineEdit				*page1_searchLineEdit;
	QComboBox				*page1_searchComboBox;
	QPushButton				*page1_searchBtn;
	QPushButton				*page1_editBtn;
	QPushButton				*page1_pwdBtn;

	QHBoxLayout				*page1_searchHbox;
	QHBoxLayout				*page1_editHBox;
	QVBoxLayout				*page1_Vbox;


	void					page1_Widget();
	void					page1_Layout();
	void					userEditMode();
	/*-------------------------------------------------------------*/
	bool					page2_EditLock = false;
	bool					page2_updateLock = false;				
	bool					page2_delteLock = false;
	QTableWidget			*page2_tableWidget;
	QLineEdit				*page2_searchLineEdit;
	QComboBox				*page2_searchComboBox;
	QPushButton				*page2_searchBtn;
	QPushButton				*page2_editBtn;
	QPushButton				*page2_deleteBtn;

	QHBoxLayout				*page2_searchHbox;
	QVBoxLayout				*page2_LeftVbox;

	

	QHBoxLayout				*page2_HBox;

	void					page2_Widget();
	void					page2_Layout();
	void					groupEditMode();
	/*-------------------------------------------------------------*/
	QSpacerItem				*hSpacer;
	QVBoxLayout				*addBtnVbox;
	QHBoxLayout				*Hbox;

	void					totalLayout();

	QString					intToQString(int);
	QString					timeStampToQString(int);
	QString					charToQString(char*);
	QString					intToNumber(int);
	QString					intStatus(int);
	

private slots:
	void					refreshData();
	void					showAdd();
	void					addNewUser();
	void					addNewDepartment();

	void					UserSearch();
	void					editUser();
	void					updateUserDate(QTableWidgetItem*);
	void					setpwdBtnEnble(QTableWidgetItem*);
	void					resetPassword();

	void					searchGroup();
	void					editGroup();
	void					deleteGroup();
	void					updataGroupDate(QTableWidgetItem*);
	void					setDeleteBtnEnble(QTableWidgetItem*);

	void					saveItemData(QTableWidgetItem*);

	
signals:

	
};

#endif
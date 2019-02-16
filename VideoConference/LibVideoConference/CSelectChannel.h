#ifndef CSELECTCHANNEL_H
#define CSELECTCHANNEL_H
#include"qwidget.h"
#include <QKeyEvent>
#include"qlistwidget.h"
#include"qtablewidget.h"
#include"qcheckbox.h"
#include"sc.h"
#include"qpushbutton.h"
#include"qlayout.h"
#include"qdebug.h"
#include "set"
#include <../LibVideoConference/SQLService.h>
class CSelectChannel :public QWidget{
	Q_OBJECT
public:
	CSelectChannel(int* idlist, std::map<int, CUserData>* memberlist);

	QTableWidget *tablew;
	void updateList();
	int cnt;
	int checklist[100];
	int desklist[100];//正在顯示的桌面(idlist and memberlist)

	QPushButton* okBtn;
	QVBoxLayout* vlay;

	int* idlistExPtr;            //儲存外部陣列位址
	std::map<int, CUserData>* memberlistExPtr;

public slots:
	void okBtn_clicked();
signals:
	void ok();
};

#endif // !CFULLDESKVIEW_H

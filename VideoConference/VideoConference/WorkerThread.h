#ifndef WORKTHREAD
#define WORKTHREAD


#include <QThread>
#include<qmessagebox.h>
#include<qcoreapplication.h>



class WorkerThread : public QThread
{
	Q_OBJECT
private:
	
public:
	QMessageBox *ms;
	QMessageBox *timeoutms;
	//WorkerThread(QObject *parent = 0) : QThread(parent){ms = new QMessageBox(QMessageBox::NoIcon, "client", QStringLiteral("正在連線到伺服器.."), QMessageBox::Yes | QMessageBox::No, NULL); }
	WorkerThread(QObject *parent = 0) : QThread(parent){ 
		ms = new QMessageBox(QMessageBox::NoIcon, "client", QStringLiteral("正在連線到伺服器 請稍候..."), NULL);
		timeoutms = new QMessageBox(QMessageBox::NoIcon, "client", QStringLiteral("遠端伺服器未開啟,將自動連接到本機伺服器!"), NULL);
		timeoutms->hide();
	}
protected:
	void run(){
		
		//Sleep(5000);

		emit done();
	}
signals:
	void done();
};
#endif 

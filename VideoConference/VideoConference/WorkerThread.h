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
	//WorkerThread(QObject *parent = 0) : QThread(parent){ms = new QMessageBox(QMessageBox::NoIcon, "client", QStringLiteral("���b�s�u����A��.."), QMessageBox::Yes | QMessageBox::No, NULL); }
	WorkerThread(QObject *parent = 0) : QThread(parent){ 
		ms = new QMessageBox(QMessageBox::NoIcon, "client", QStringLiteral("���b�s�u����A�� �еy��..."), NULL);
		timeoutms = new QMessageBox(QMessageBox::NoIcon, "client", QStringLiteral("���ݦ��A�����}��,�N�۰ʳs���쥻�����A��!"), NULL);
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

#ifndef CFILEEXPLORER_H
#define CFILEEXPLORER_H
#include<QDir>
#include<qstringlist.h>
#include<qfileinfo.h>
#include<QFileInfoList>
#include<qdebug.h>
#include<qwidget.h>
#include <QListWidget>
#include<qfiledialog.h>
#include<qlabel.h>
#include<qlayout.h>
#include <QProcess>
#include<qdesktopservices.h>
#include<qmessagebox.h>
#include<qpushbutton.h>
#include"sc.h"

class CFileExplorer: public QWidget{
    Q_OBJECT
public:
    CFileExplorer();
    void scan();
    QListWidget *listw;
    QLabel *flab;
    QVBoxLayout *vlay;
    QString fn;
	QPushButton* openBtn;
public slots:
    void open(QModelIndex item);
};

#endif // CFILEEXPLORER_H

#include"CFileExplorer.h"
#define qs QString::fromLocal8Bit
CFileExplorer::CFileExplorer(){
	openBtn = new QPushButton(qs("開啟路徑"),this);
    listw=new QListWidget(this);
    flab = new QLabel("label",this);
    QObject::connect(listw, SIGNAL(currentTextChanged(const QString &)),flab, SLOT(setText(const QString &)));
    QObject::connect(listw, SIGNAL(doubleClicked(QModelIndex)),this, SLOT(open(QModelIndex)));
    vlay = new QVBoxLayout(this);
    vlay->addWidget(flab);
    vlay->addWidget(listw);
	vlay->addWidget(openBtn);
	QObject::connect(openBtn, &QPushButton::clicked, [=](){scan(); });
}
void CFileExplorer::scan(){

    fn = QFileDialog::getExistingDirectory(this);
    QDir dir;
    dir.setPath(fn);
    qDebug()<<fn;
    QStringList filters;
    filters << "*.*";



    dir.setFilter( QDir::Files | QDir::Hidden | QDir::NoSymLinks );

    dir.setSorting( QDir::Size | QDir::Reversed );

    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList();
    QFileInfo fi;
    qDebug()<<"Bytes Filename";

    for(int i = 0; i < list.count(); i++)
    {
    fi = list.at(i);
    qDebug()<<"file size:"<<fi.size()<<", file name:"<<fi.fileName();
    listw->insertItem(0, new QListWidgetItem(fi.fileName()));
    }
}
void CFileExplorer::open(QModelIndex item)
{
    QString full;
    QString n;
    n=item.data().toString();
    full+=fn;
    full+='/';
    full+=n;
    qDebug()<<full;

     QStringList sl;
    //QProcess::execute(full,sl); 啟動後立即返回
    //QProcess::startDetached() 阻塞直到外部程式結束後返回 需使用QProcess QThread防止阻塞
    QDesktopServices ds;
    if(!ds.openUrl(QUrl::fromLocalFile(full))) QMessageBox::information(this,qs("開啟檔案發生錯誤"),full);

}

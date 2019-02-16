#include "FDialog.h"

#define qs QStringLiteral
FDialog::FDialog(QWidget *parent) :
    QDialog(parent)
{

    loadSize = 4*1024;
    totalBytes=0;
    bytesWritten = 0;
    bytesToWrite = 0;
    clientProgressBar = new QProgressBar;
    clientStatusLabel = new QLabel(QStringLiteral(""));
    startButton = new QPushButton(QStringLiteral("開始"));
    openButton = new QPushButton(QStringLiteral("開檔"));
    quitButton = new QPushButton(QStringLiteral("退出"));
    startButton->setEnabled(false);
    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    buttonBox->addButton(startButton,QDialogButtonBox::ActionRole);
    buttonBox->addButton(openButton,QDialogButtonBox::ActionRole);
    //buttonBox->addButton(quitButton,QDialogButtonBox::RejectRole);  //TBD
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(clientProgressBar);
    mainLayout->addWidget(clientStatusLabel);
    mainLayout->addStretch(1);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);
    setWindowTitle(QStringLiteral("檔案傳送客戶端"));

    ipEdit=new QLineEdit("127.0.0.1",this);
    portEdit=new QLineEdit("16998",this);
    mainLayout->addWidget(ipEdit);
    mainLayout->addWidget(portEdit);

    connect(startButton,SIGNAL(clicked()),this,SLOT(start()));
    connect(openButton,SIGNAL(clicked()),this,SLOT(openFile()));
    connect(quitButton,SIGNAL(clicked()),this,SLOT(close()));
    connect(&tcpClient,SIGNAL(connected()),this,SLOT(startTransfer()));
    connect(&tcpClient,SIGNAL(bytesWritten(qint64)),this,SLOT(updateClientProcess(qint64)));
}

FDialog::~FDialog()
{

}
void FDialog::start()// 使用者按開始按鈕
{
    startButton->setEnabled(false);//禁止使用者重複按開始鈕
    bytesWritten=0;
    clientStatusLabel->setText(QStringLiteral("連線中..."));
    QString ip;
    ip=ipEdit->text();
    QString portc;
    portc=portEdit->text();
    int port=portc.toInt();
    tcpClient.connectToHost(QHostAddress(ip),port);
}
void FDialog::startTransfer()//處理TCP連線功能
{
    localFile = new QFile(fileName);
    if(!localFile->open(QFile::ReadOnly))
    {
        QMessageBox::warning(this,QStringLiteral("應用程式"),QStringLiteral("無法讀取 %1\n%2.").arg(fileName).arg(localFile->errorString()));

        return;
    }
    totalBytes=localFile->size();//傳回檔案大小(bytes)
    QDataStream sendOut(&outBlock,QIODevice::WriteOnly);
    sendOut.setVersion(QDataStream::Qt_4_6);//設定資料格式版本號
    QString currentFile=fileName.right(fileName.size()-fileName.lastIndexOf("/")-1);
    //QString::currentFile=fileName.section(-1,-1,0);
    sendOut<<qint64(0)<<qint64(0)<<currentFile;
    totalBytes+=outBlock.size();//檔名長度大小+檔案長度大小+檔名大小
    sendOut.device()->seek(0);//輸出指標移到頭端
    sendOut<<totalBytes<<qint64((outBlock.size()-sizeof(qint64)*2));//總長度<<檔案名稱長度
    bytesToWrite=totalBytes-tcpClient.write(outBlock);//尚未傳送的byte數
    clientStatusLabel->setText(qs("已連線"));
    outBlock.resize(0);//清空OutBlock


}
void FDialog::updateClientProcess(qint64 numBytes)//處理TCP每筆資料(4KB)傳輸成功後
{
       bytesWritten+=(int)numBytes;
       if(bytesToWrite>0)
       {
           //從檔案讀取RawData,儲存在outBlock中
            outBlock=localFile->read(qMin(bytesToWrite,loadSize));
            //將outBlock中的資料寫出去(寫到網路)
            bytesToWrite-=(int)tcpClient.write(outBlock);
            outBlock.resize(0);
       }
       else
       {
           localFile->close();
       }
       clientProgressBar->setMaximum(totalBytes);
       clientProgressBar->setValue(bytesWritten);
       clientStatusLabel->setText(qs("已傳送 %1 Bytes").arg(bytesWritten));
}
void FDialog::openFile()//處理使用者按開檔案鈕
{
    fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty())startButton->setEnabled(true);
}

#ifndef FDIALOG_H
#define FDIALOG_H

#include <QDialog>
#include <QtWidgets/QtWidgets>
#include <QtNetwork>
#include<QLineEdit>


class FDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FDialog(QWidget *parent = 0);
    ~FDialog();

private:
    QProgressBar *clientProgressBar;
    QLabel *clientStatusLabel;
    QPushButton *startButton;
    QPushButton *openButton;
    QPushButton *quitButton;
    QTcpSocket tcpClient;

    qint64 totalBytes;//紀錄資料總長度
    qint64 bytesWritten;//紀錄目前已寫出資料長度
    qint64 bytesToWrite;//紀錄目前尚未寫出資料長度
    qint64 loadSize;//紀錄每筆資料長度
    QString fileName;//就是filename
    QFile *localFile;
    QByteArray outBlock;//資料輸出儲存緩衝區
    QLineEdit* ipEdit;
    QLineEdit* portEdit;

public slots:
    void start();// 使用者案開始按鈕
    void startTransfer();//處理TCP連線功能
    void updateClientProcess(qint64);//處理TCP每筆資料(4KB)傳輸成功後
    void openFile();//處理使用者按開檔案紐


};

#endif // FDIALOG_H

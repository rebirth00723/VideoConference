#ifndef CLIENTDIALOGSTRING_H
#define CLIENTDIALOGSTRING_H

#include <QDialog>
#include <QHostAddress>
#include <QTcpSocket>
#include<QLabel>

class QProgressBar;
class QPushButton;
class QFile;

class ClientDialogString : public QDialog {
	Q_OBJECT

public:
	ClientDialogString(QWidget *parent = 0);
	void setHostAddressAndPort(QString hostAddress, quint16 port);
	void closeConnection();
QString name;
QString dir;
	public slots:
	void start();
	void startTransfer();
	void updateProgress(qint64 numBytes);
	void displayError(QAbstractSocket::SocketError socketError);

	
private:
	QProgressBar *progressBar;
	QPushButton *selectBtn;

	QHostAddress hostAddress;
	quint16 hostPort;
	QTcpSocket client;

	QFile *file;
	
	QLabel* filelabel;

};

#endif
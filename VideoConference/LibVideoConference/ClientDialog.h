#ifndef CLIENTDIALOG_H
#define CLIENTDIALOG_H

#include <QDialog>
#include <QHostAddress>
#include <QTcpSocket>
#include"ClientDialogString.h"
class QProgressBar;
class QPushButton;
class QFile;

class ClientDialog : public QDialog {
	Q_OBJECT

public:
	ClientDialog(QWidget *parent = 0);
	void setHostAddressAndPort(QString hostAddress, quint16 port);
	void closeConnection();

	public slots:
	void start();
	void startTransfer();
	void updateProgress(qint64 numBytes);
	void displayError(QAbstractSocket::SocketError socketError);

private:
	QProgressBar *progressBar;
	QPushButton *startBtn;

	QHostAddress hostAddress;
	quint16 hostPort;
	QTcpSocket client;

	QFile *file;
	QString filename;
	ClientDialogString* cds;
};

#endif
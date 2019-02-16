#ifndef SERVERDIALOG_H
#define SERVERDIALOG_H

#include <QDialog>
#include <QTcpServer>

class QLabel;
class QFile;
class QTcpSocket;

class ServerDialog : public QDialog {
	Q_OBJECT

public:
	ServerDialog(QWidget *parent = 0);
	void setReceivedFileName(QString fileName);
	void listen(quint16 port);

	public slots:
	void acceptConnection();
	void updateProgress();
	void displayError(QAbstractSocket::SocketError socketError);

private:
	QLabel *label;
	QTcpServer server;
	QTcpSocket *clientConnection;
	int bytesReceived;
	QFile *file;
	QString dname;
};

#endif
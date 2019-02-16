#include <QApplication>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHostAddress>
#include <QLabel>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QMessageBox>
#include <QFile>

#include <iostream>
using namespace std;

#include "ServerDialog.h"

ServerDialog::ServerDialog(QWidget *parent) : QDialog(parent) {
	label = new QLabel("Received:");
	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(label);
	this->setLayout(layout);

	connect(&server, SIGNAL(newConnection()),this, SLOT(acceptConnection()));
}

void ServerDialog::setReceivedFileName(QString fileName) {
	file = new QFile(fileName);
}

void ServerDialog::listen(quint16 port) {
	server.listen(QHostAddress::Any, port);
}

void ServerDialog::acceptConnection() {
	if (!file->open(QIODevice::WriteOnly)) {
		cerr << "Unable to write the file" << endl;
		delete file;
		file = 0;
		return;
	}

	clientConnection = server.nextPendingConnection();

	connect(clientConnection, SIGNAL(readyRead()),this, SLOT(updateProgress()));
	
	connect(clientConnection, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));

	server.close();

	//QApplication::setOverrideCursor(Qt::WaitCursor);
}

void ServerDialog::updateProgress() {
	//bytesReceived += (int)clientConnection->bytesAvailable();
	file->write(clientConnection->readAll());

	//QString txt = "Received %1MB";

	//label->setText(txt.arg(bytesReceived / (1024 * 1024)));
}

void ServerDialog::displayError(QAbstractSocket::SocketError socketError) {
	file->close();

	if (socketError == QTcpSocket::RemoteHostClosedError) {
		QMessageBox::information(this,"OK!", "Save file as " + file->fileName());
	}
	else {
		QMessageBox::information(this, "Network error",
			"The following error occurred: " + clientConnection->errorString());
	}

	delete file;
	file = 0;

	QApplication::restoreOverrideCursor();
}
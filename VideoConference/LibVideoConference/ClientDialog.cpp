#include <QApplication>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
using namespace std;

#include "ClientDialog.h"

ClientDialog::ClientDialog(QWidget *parent) : QDialog(parent) {
	progressBar = new QProgressBar;
	progressBar->setValue(0);

	startBtn = new QPushButton("Start");

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(progressBar);
	layout->addWidget(startBtn);
	this->setLayout(layout);

	connect(startBtn, SIGNAL(clicked()), this, SLOT(start()));

	connect(&client, SIGNAL(connected()), this, SLOT(startTransfer()));

	connect(&client, SIGNAL(bytesWritten(qint64)),this, SLOT(updateProgress(qint64)));

	connect(&client, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));
}

void ClientDialog::setHostAddressAndPort(QString address, quint16 port) {
	hostAddress.setAddress(address);
	this->hostPort = port;
}

void ClientDialog::start() {
	QString fileName = QFileDialog::getOpenFileName(
		this, "Open File", "F:\\", "All Files (*.*)");
	if (fileName == NULL) {
		return;
	}
	file = new QFile(fileName);

	startBtn->setEnabled(false);

	client.connectToHost(hostAddress, hostPort);

	QApplication::setOverrideCursor(Qt::WaitCursor);
}

void ClientDialog::startTransfer() {
	if (!file->open(QIODevice::ReadOnly)) {
		cerr << "Unable to read the file" << endl;
		delete file;
		file = 0;
		return;
	}

	client.write(file->readAll());
}

void ClientDialog::updateProgress(qint64 numBytes) {
	int written = progressBar->value() + (int)numBytes;

	progressBar->setMaximum(file->size());
	progressBar->setValue(written);

	if (progressBar->value() == progressBar->maximum()) {
		closeConnection();
	}
}

void ClientDialog::closeConnection() {
	client.close();

	file->close();
	delete file;
	file = 0;

	progressBar->reset();
	startBtn->setEnabled(true);

	QApplication::restoreOverrideCursor();
}

void ClientDialog::displayError(QAbstractSocket::SocketError socketError) {
	QMessageBox::information(this, "Network error",
		"The following error occurred: " + client.errorString());
	closeConnection();
}
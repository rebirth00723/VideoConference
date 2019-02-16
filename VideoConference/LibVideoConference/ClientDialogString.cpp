#include <QApplication>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>
using namespace std;

#include "ClientDialogString.h"

ClientDialogString::ClientDialogString(QWidget *parent) : QDialog(parent) {
	progressBar = new QProgressBar;
	progressBar->setValue(0);

	selectBtn = new QPushButton("Select File to Upload");
	filelabel = new QLabel("SelectedFile");

	QVBoxLayout *layout = new QVBoxLayout;
	//layout->addWidget(progressBar);
	layout->addWidget(selectBtn);
	layout->addWidget(filelabel);
	this->setLayout(layout);

	connect(selectBtn, SIGNAL(clicked()), this, SLOT(start()));
	connect(&client, SIGNAL(connected()), this, SLOT(startTransfer()));
	//connect(&client, SIGNAL(bytesWritten(qint64)),this, SLOT(updateProgress(qint64)));
	connect(&client, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(displayError(QAbstractSocket::SocketError)));
}

void ClientDialogString::setHostAddressAndPort(QString address, quint16 port) {
	hostAddress.setAddress(address);
	this->hostPort = port;
}

void ClientDialogString::start() {
	QString fileName = QFileDialog::getOpenFileName(
		this, "Open File", "F:\\", "All Files (*.*)");
	if (fileName == NULL) {
		return;
	}

	file = new QFile(fileName);
	dir = fileName;
	name = fileName;
	name = name.section('/', -1, -1);
	filelabel->setText(name);

	selectBtn->setEnabled(false);

	client.connectToHost(hostAddress, hostPort);

	QApplication::setOverrideCursor(Qt::WaitCursor);
}

void ClientDialogString::startTransfer() {
	if (!file->open(QIODevice::ReadOnly)) {
		cerr << "Unable to read the file" << endl;
		delete file;
		file = 0;
		return;
	}

	//client.write(file->readAll());
	client.write(name.toLatin1());
	closeConnection();
}


void ClientDialogString::updateProgress(qint64 numBytes) {
	int written = progressBar->value() + (int)numBytes;

	progressBar->setMaximum(file->size());
	progressBar->setValue(written);

	if (progressBar->value() == progressBar->maximum()) {
		closeConnection();
	}
}

void ClientDialogString::closeConnection() {
	client.close();

	file->close();
	delete file;
	file = 0;

	progressBar->reset();
	selectBtn->setEnabled(true);

	QApplication::restoreOverrideCursor();
}

void ClientDialogString::displayError(QAbstractSocket::SocketError socketError) {
	QMessageBox::information(this, "Network error",
		"The following error occurred: " + client.errorString());
	closeConnection();
}
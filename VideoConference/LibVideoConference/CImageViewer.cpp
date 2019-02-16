#include"CImageViewer.h"

#define imbufsize 720000
unsigned long zlen;
unsigned long ulen;

char* ubuf = (char*)malloc(imbufsize);

char* sbuf = (char*)malloc(imbufsize);
char* zbuf = (char*)malloc(imbufsize);

char* recvBuf = (char*)malloc(imbufsize);

char* p1buf = (char*)malloc(imbufsize);
char* p2buf = (char*)malloc(imbufsize);
char* p3buf = (char*)malloc(imbufsize);
char* p4buf = (char*)malloc(imbufsize);
char* p5buf = (char*)malloc(imbufsize);
char* p6buf = (char*)malloc(imbufsize);
int grabsize;
CImageViewer* mthis;
bool first=true;


void CImageViewer::recvim()
{
	while (udp->hasPendingDatagrams())
	{
		
		udp->readDatagram(p1buf, image.byteCount());
		QImage *im = new QImage((uchar*)p1buf, image.width(), image.height()/6, QImage::Format_RGB32);

		imagePart1 = im->copy();

		update();
		
	}
}
void CImageViewer::recvim2()
{
	while (udp2->hasPendingDatagrams())
	{
		udp2->readDatagram(p2buf, image.byteCount());

		QImage *im = new QImage((uchar*)p2buf, image.width(), image.height()/6, QImage::Format_RGB32);
		imagePart2 = im->copy();
		update();
		
	}
}
void CImageViewer::recvim3()
{
	while (udp3->hasPendingDatagrams())
	{
		udp3->readDatagram(p3buf, image.byteCount());

		QImage *im = new QImage((uchar*)p3buf, image.width(), image.height() / 6, QImage::Format_RGB32);
		imagePart3 = im->copy();
		update();

	}
}
void CImageViewer::recvim4()
{
	while (udp4->hasPendingDatagrams())
	{
		udp4->readDatagram(p4buf, image.byteCount());

		QImage *im = new QImage((uchar*)p4buf, image.width(), image.height() / 6, QImage::Format_RGB32);
		imagePart4 = im->copy();
		update();

	}
}
void CImageViewer::recvim5()
{
	while (udp5->hasPendingDatagrams())
	{
		udp5->readDatagram(p5buf, image.byteCount());

		QImage *im = new QImage((uchar*)p5buf, image.width(), image.height() / 6, QImage::Format_RGB32);
		imagePart5 = im->copy();
		update();

	}
}
void CImageViewer::recvim6()
{
	while (udp6->hasPendingDatagrams())
	{
		udp6->readDatagram(p6buf, image.byteCount());

		QImage *im = new QImage((uchar*)p6buf, image.width(), image.height() / 6, QImage::Format_RGB32);
		imagePart6 = im->copy();
		update();

	}
}

CImageViewer::CImageViewer(QWidget *parent)
{
	mthis = this;

	QPixmap p = QPixmap::grabWindow(this->winId());
	image = p.toImage();
	image.fill(Qt::white);
	image.save("ii.jpg");

	//udp = new UDPSocket("225.6.7.8", 4444);
	/////////////////////////////////////////////
	udp = new QUdpSocket;
	udp2 = new QUdpSocket;
	udp3 = new QUdpSocket;
	udp4 = new QUdpSocket;
	udp5 = new QUdpSocket;
	udp6 = new QUdpSocket;

	udp->bind(QHostAddress::AnyIPv4, 12121, QUdpSocket::ShareAddress);
	udp2->bind(QHostAddress::AnyIPv4, 6666, QUdpSocket::ShareAddress);
	udp3->bind(QHostAddress::AnyIPv4, 7777, QUdpSocket::ShareAddress);
	udp4->bind(QHostAddress::AnyIPv4, 8888, QUdpSocket::ShareAddress);
	udp5->bind(QHostAddress::AnyIPv4, 14442, QUdpSocket::ShareAddress);
	udp6->bind(QHostAddress::AnyIPv4, 11111, QUdpSocket::ShareAddress);
	if (udp->joinMulticastGroup(QHostAddress("225.6.7.8")))
	{
		udp->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 600 * 300 * 10000);
		udp->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 600 * 300 * 10000);
		connect(udp, SIGNAL(readyRead()), this, SLOT(recvim()));
		
	}
	
	if (udp2->joinMulticastGroup(QHostAddress("225.6.7.8")))
	{
		udp2->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 600 * 300 * 10000);
		udp2->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 600 * 300 * 10000);
		connect(udp2, SIGNAL(readyRead()), this, SLOT(recvim2()));
	}
	if (udp3->joinMulticastGroup(QHostAddress("225.6.7.8")))
	{
		udp3->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 600 * 300 * 10000);
		udp3->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 600 * 300 * 10000);
		connect(udp3, SIGNAL(readyRead()), this, SLOT(recvim3()));
	}
	if (udp4->joinMulticastGroup(QHostAddress("225.6.7.8")))
	{
		udp4->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 600 * 300 * 10000);
		udp4->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 600 * 300 * 10000);
		connect(udp4, SIGNAL(readyRead()), this, SLOT(recvim4()));
	}
	if (udp5->joinMulticastGroup(QHostAddress("225.6.7.8")))
	{
		udp5->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 600 * 300 * 10000);
		udp5->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 600 * 300 * 10000);
		connect(udp5, SIGNAL(readyRead()), this, SLOT(recvim5()));
	}
	if (udp6->joinMulticastGroup(QHostAddress("225.6.7.8")))
	{
		udp6->setSocketOption(QAbstractSocket::ReceiveBufferSizeSocketOption, 600 * 300 * 10000);
		udp6->setSocketOption(QAbstractSocket::SendBufferSizeSocketOption, 600 * 300 * 10000);
		connect(udp6, SIGNAL(readyRead()), this, SLOT(recvim6()));
	}
	
	//////////////////////////////////////////////
	QObject::connect(&timer, &QTimer::timeout, [=](){update(); });
	timer.start(100);

}

void CImageViewer::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e);
	QPainter painter(this);
	if (ii) {
		painter.drawImage(QPoint(0, 0), image);
		ii = false;
		return;
	}
	painter.drawImage(QPoint(0, 0), imagePart1);
	painter.drawImage(QPoint(0, 25), imagePart2);
	painter.drawImage(QPoint(0, 50), imagePart3);
	painter.drawImage(QPoint(0, 75), imagePart4);
	painter.drawImage(QPoint(0, 100), imagePart5);
	painter.drawImage(QPoint(0, 125), imagePart6);
	
	painter.setPen(QPen(color, painterSize));

	if (drawDone){
		p = QPixmap::grabWindow(this->winId());
		image = p.toImage();
		
		memcpy(sbuf, image.bits(), image.byteCount());
		if (image.byteCount() > 0)
		{	
			

			udp->writeDatagram(sbuf, 60000, QHostAddress("225.6.7.8"), (quint16)12121);
			udp2->writeDatagram(sbuf + 60000, 60000, QHostAddress("225.6.7.8"), (quint16)6666);
			udp3->writeDatagram(sbuf + 120000, 60000, QHostAddress("225.6.7.8"), (quint16)7777);
			udp4->writeDatagram(sbuf + 180000, 60000, QHostAddress("225.6.7.8"), (quint16)8888);
			udp5->writeDatagram(sbuf + 240000, 60000, QHostAddress("225.6.7.8"), (quint16)14442);
			udp6->writeDatagram(sbuf + 300000, 60000, QHostAddress("225.6.7.8"), (quint16)11111);
		
		}
	}
	p = QPixmap::grabWindow(this->winId());
	image = p.toImage();

	QImage scaledImg = image.scaled(this->size(), Qt::IgnoreAspectRatio);
	//painter.drawImage(QPoint(0, 0), scaledImg);

	switch (currentMode)
	{
	case drawMode::rect:
		if (drawDone)painter.drawRect(startPoint.rx(), startPoint.ry(), endPoint.rx() - startPoint.rx(), endPoint.ry() - startPoint.ry());
	
		break;
	case drawMode::circle:
		if (drawDone)painter.drawEllipse(startPoint.rx(), startPoint.ry(), endPoint.rx() - startPoint.rx(), endPoint.ry() - startPoint.ry());

		break;
	case drawMode::line:
		if (drawDone)painter.drawLine(startPoint, endPoint);

		break;
	case drawMode::clear:
		painter.eraseRect(0, 0, this->width(), this->height());
		break;	
	case drawMode::pen:
		if (drawDone)
		{
			painter.drawLine(lastPoint, endPoint);
			lastPoint = endPoint;	
		}
		break;
	case drawMode::image:
		QRectF target(startPoint,endPoint);
		if (drawDone) painter.drawImage(target, brushImage);

		break;
	}
}
void CImageViewer::updateImage(QImage im)
{
	image = im.copy();
	update();    //import!! you should uncomment this line if you want to use CImageViewer singlely;
}
void CImageViewer::mousePressEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton) 
		drawDone = false;
		lastPoint = e->pos();
	startPoint = e->pos();
}
void CImageViewer::mouseMoveEvent(QMouseEvent *e)
{
	if (e->button() == Qt::LeftButton)
	{
		drawDone = true;/////////
	endPoint = e->pos();
	update();
	}
}
void CImageViewer::mouseReleaseEvent(QMouseEvent*e)
{
	if (e->button() == Qt::LeftButton) {
		drawDone = true;
	endPoint = e->pos();
	e->accept();
	}
	this->update();
}
/*destructor
udp->disconnected();
delete udp;
udp = nullptr;
*/
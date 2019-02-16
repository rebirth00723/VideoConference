#ifndef CImageViewer_H
#define CImageViewer_H
#include"qpainter.h"
#include"qtimer.h"
#include"qwidget.h"
#include"qimage.h"
#include"sc.h"
#include"qevent.h"
#include"qpixmap.h"
#include"qscreen.h"
#include"qguiapplication.h"
#include"UDPSocket.h"
#include"qudpsocket.h"
#include"qthread.h"
#include"QtZlib\zlib.h"

enum drawMode{
	pen,
	line,
	rect,
	circle,
	image,
	text,
	clear,
};
class CImageViewer :public QWidget
{
	Q_OBJECT
public:
	CImageViewer(QWidget *parent = 0);
	void paintEvent(QPaintEvent*);
	QImage image;
	QImage brushImage;
	int currentMode=0;
	QColor color;
	QPoint startPoint;
	QPoint endPoint;
	QPoint lastPoint;
	QTimer timer;
	QTimer timer2;
	bool drawDone = false;
	QScreen *screen;
	QPixmap p;
	QPixmap pix;
	int painterSize=1;

	//UDPSocket *udp;
	QUdpSocket *udp;
	QUdpSocket *udp2;
	QUdpSocket *udp3;
	QUdpSocket *udp4;
	QUdpSocket *udp5;
	QUdpSocket *udp6;

	void mouseMoveEvent(QMouseEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void updateImage(QImage);

	QImage imagePart1;
	QImage imagePart2;
	QImage imagePart3;
	QImage imagePart4;
	QImage imagePart5;
	QImage imagePart6;

	bool ii = true;
	public slots:
	void recvim();
	void recvim2();
	void recvim3();
	void recvim4();
	void recvim5();
	void recvim6();

private:
	
	QPainter painter;
};
#endif // !CImageViewer

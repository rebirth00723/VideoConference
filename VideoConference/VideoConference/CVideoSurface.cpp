#include"CVideoSurface.h"
#include"iostream"
using namespace std;
#define qs QStringLiteral
CVideoSurface::CVideoSurface(int width, int height,bool showFPS,bool showMbps)
{
	w = width;
	h = height;
	this->setMinimumSize(w, h);
	show_fps = showFPS;
	show_mbps = showMbps;
	frame_num = 0;
	frame_num_temp = 0;
	fps_isCounting = true;
	mbps_isCounting = true;
	firstrun = true;
	is_stabled = false;
	bytecount_temp = 0;
	bytecount = 0;
	timer = new QTimer();
	timer2 = new QTimer();
	QObject::connect(timer, &QTimer::timeout, [=](){fps_isCounting = false;});
	QObject::connect(timer2, &QTimer::timeout, [=](){mbps_isCounting = false;});
	timer->start(200);
	timer2->start(1000);
}

void CVideoSurface::paintEvent(QPaintEvent *e)
{
	Q_UNUSED(e);
	this->setMinimumSize(0, 0);

	QPainter painter(this);
	painter.setPen(QPen(Qt::yellow, 5));
	painter.setFont(QFont("Times New Roman", 40));	

	QImage scaledImg = image.scaled(this->size(), Qt::IgnoreAspectRatio);


	QString fps = "FPS:";
	fps += QString::number(frame_num);

	QString mbps = QString::number(bytecount);
	mbps += "Mbps";

	if (frame_num > 50)
	{
		firstrun = false;
	}
	if (frame_num < 40 && !firstrun)
	{
		is_stabled = true;
	}
	if (scaledImg.bits()>0)//is_stabled&&
	{
		painter.drawImage(QPoint(0, 0), scaledImg);
		if (show_fps)painter.drawText(0, 0, this->width(), this->height(), Qt::AlignTop, fps);
		if (show_mbps)painter.drawText(this->width()/2, 0, this->width(), this->height(), Qt::AlignTop, mbps);
	}

	else
	{
		painter.setPen(QPen(Qt::black, 5));
		painter.setBrush(Qt::black);
		painter.drawRect(0, 0, this->width(), this->height());
		painter.setPen(QPen(Qt::green, 5));
		painter.setFont(QFont("Arial", 80));
		painter.drawText(0, 0, this->width(), this->height(), Qt::AlignCenter, qs("正在初始化..."));
	}

	if (!fps_isCounting)
	{
		frame_num = frame_num_temp*5;
		frame_num_temp = 0;
		fps_isCounting = true;
	}
	if (!mbps_isCounting)
	{
		bytecount = (bytecount_temp/8)/(2^20*100);
		bytecount_temp = 0;
		mbps_isCounting = true;
	}

	/*
	QRectF source(0, 0, 640, 480);

	QRectF target(0, 0, 640, 480);

	painter.drawImage(target, image, source);
	*/
}
void CVideoSurface::updateImage(QImage im)
{
	image = im.copy();
	frame_num_temp++;
	//this->update();    //import!! you should uncomment this line if you want to use cvideosurface singlely;
}
void CVideoSurface::updateImageRefresh(QImage im)
{
	image = im.copy();
	frame_num_temp++;
	this->update();
}
void CVideoSurface::updateMbps(int bytecount)
{
	bytecount_temp += bytecount;
}
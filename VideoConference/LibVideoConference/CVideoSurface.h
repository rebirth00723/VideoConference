#ifndef CVideoSurface_H
#define CVideoSurface_H
#include"qpainter.h"
#include"qtimer.h"
#include"qwidget.h"
#include"qimage.h"
class CVideoSurface :public QWidget
{
	Q_OBJECT
public:
	CVideoSurface(int width, int height, bool showFPS=false,bool showMbps=false);
	void paintEvent(QPaintEvent*);
	void updateMbps(int bytecount);
	QImage image;
public slots:
	void updateImage(QImage);
	void updateImageRefresh(QImage);
private:
	int w;
	int h;
	QPainter painter;
	QTimer* timer;
	QTimer* timer2;
	bool show_fps;
	bool show_mbps;
	bool fps_isCounting;
	bool mbps_isCounting;
	int frame_num;
	int frame_num_temp;
	bool is_stabled;
	double bytecount_temp;
	double bytecount;
	bool firstrun;
};
#endif // !CVideoSurface


#ifndef CNETPAINTER_H
#define CNETPAINTER_H
#include"qwidget.h"
#include"qpainter.h"
#include"qtimer.h"
#include"qimage.h"
#include"sc.h"

#include"CImageViewer.h"
#include"qlabel.h"
#include"qlayout.h"
#include"qpushbutton.h"
#include"qcolordialog.h"
#include"qslider.h"
#include"qfiledialog.h"
#include <QFrame>
#include"qtimer.h"
#define UPDATETIME 500

/*	
ADD code before you use it singlely:

WSADATA wsad;
WSAStartup(MAKEWORD(2, 2), &wsad);
CNetPainter p;
*/
class CNetPainter:public QWidget{
	Q_OBJECT
public:
	CNetPainter();
	QVBoxLayout *mainlay;
	QHBoxLayout *toollay;
	QPushButton *setColorBtn;
	QPushButton *addImageBtn;
	QPushButton *addImageBackgroundBtn;
	QPushButton *rectBtn;
	QPushButton *circleBtn;
	QPushButton *lineBtn;
	QPushButton *clearBtn;
	CImageViewer *view;
	QFrame *colorFrame;
	QColor c;
	QSlider *slider;
	QLabel* sizeLabel;
	QTimer timer;
public slots:
	void addImage();
	void addImageBackground();
	void setColorBtn_clicked();
	void rectBtn_clicked();
	void circleBtn_clicked();
	void lineBtn_clicked();
	void clearBtn_clicked();
	void setPainterSize(int);
};

#endif // !CNETPAINTER_H


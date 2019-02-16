#ifndef cdeskview_H
#define cdeskview_H
#include"qwidget.h"
#include <QKeyEvent>
class CDeskView:public QWidget{
	Q_OBJECT
public:
	void keyPressEvent(QKeyEvent *event);
signals:
	void exit();
};

#endif // !CFULLDESKVIEW_H

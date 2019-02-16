#include "ColorDefine.h"
using namespace std;

ColorDefine::ColorDefine(QWidget *parent)
{
	btnstylesheet = "QPushButton{	background - color:#000000; \
									color: #F8F8FF; \
									min-width: 6em;\
									border - radius: 10px;\
									border: 2px groove #CD6600; \
									border - style: outset; }"
		   "QPushButton:hover {		background-color:#F8F8FF; color: #000000;}";

	tabStyle = "QTabBar::tab {					min-width: 120px; \
												border: 1px solid; \
												border-top-left-radius: 10px;\
												border-top-right-radius: 10px;\
												padding:2px; \
												color: 	#000000; \
												background:#FF8C00;}\
						QTabBar::tab:!selected	{margin-top: 3px;}\
						QTabBar::tab:selected	{color: #FFFAFA;}\
						QTabWidget::pane {		border-width:5px;\
												border-color : #CD6600;\
												border-style:outset;\
												background-color : #000000 }";

	tabWidget = "background-color : #000000;\
				 color : #CD6600; ";
}
void ColorDefine::setEDMIDColor(QPalette *a, QPalette *b, QString *c)
{
	a->setColor(QPalette::Window, Qt::black);
	b->setColor(QPalette::WindowText, QColor(205, 102, 0));
	*c = btnstylesheet;
}
void ColorDefine::setLoginColor(QPalette *a, QPalette *b, QString *c)
{
	a->setColor(QPalette::Window, Qt::black);
	b->setColor(QPalette::Text, QColor(205, 102, 0));
	*c = btnstylesheet;
}
void ColorDefine::setSPColor(QPalette *a, QPalette *b, QPalette *c, QString *d)
{
	a->setColor(QPalette::Window, Qt::black);
	b->setColor(QPalette::WindowText, QColor(205, 102, 0));
	c->setColor(QPalette::Text, QColor(205, 102, 0));
	*d = btnstylesheet;
}
void ColorDefine::setClientColor(QPalette *a, QPalette *b, QPalette *c, QPalette *d, QPalette *e, QString *f, QString *g, QString *h, QString *i, QString *j)
{
	a->setColor(QPalette::Window, Qt::black);
	b->setColor(QPalette::Base, Qt::black);
	c->setColor(QPalette::WindowText, QColor(205, 102, 0));
	d->setColor(QPalette::Base, QColor(205, 102, 0));
	e->setColor(QPalette::Text, QColor(205, 102, 0));
	*f = "background-color : #00FF00";
	*g = "background-color : #FF0000";
	*h = btnstylesheet;
	*i = tabStyle;
	*j = tabWidget;
}

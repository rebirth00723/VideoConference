#include"CNetPainter.h"


CNetPainter* pthis;

CNetPainter::CNetPainter(){

	pthis = this;
	this->setFixedSize(650, 200);
	mainlay = new QVBoxLayout(this);
	toollay = new QHBoxLayout(this);
	view = new CImageViewer(this);
	view->setFixedSize(600,150);
	mainlay->addWidget(view);
	mainlay->addLayout(toollay);

	/*toolBar widgets*/
	colorFrame = new QFrame(this);
	colorFrame->setFrameShape(QFrame::Box);
	colorFrame->setAutoFillBackground(true);
	colorFrame->setFixedSize(20, 20);
	colorFrame->setPalette(QPalette(c));

	setColorBtn = new QPushButton(qs(""),this);
	setColorBtn->setIcon(QIcon("../images/pickColor.png"));
	addImageBtn = new QPushButton(qs(""),this);
	addImageBtn->setIcon(QIcon("../images/stamp.png"));
	//addImageBackgroundBtn = new QPushButton(qs("插入背景圖片"), this);   //TBD
	rectBtn = new QPushButton(qs(""), this);
	rectBtn->setIcon(QIcon("../images/rectangle.png"));
	circleBtn = new QPushButton(qs(""), this);
	circleBtn->setIcon(QIcon("../images/circle.png"));
	lineBtn = new QPushButton(qs(""), this);
	lineBtn->setIcon(QIcon("../images/line.png"));
	clearBtn = new QPushButton(qs(""), this);
	clearBtn->setIcon(QIcon("../images/clear.png"));
	slider = new QSlider(Qt::Horizontal,this);
	slider->setFixedWidth(50);
	slider->setValue(1);
	sizeLabel = new QLabel(qs("筆刷:1px"), this);

	toollay->addWidget(slider);
	toollay->addWidget(sizeLabel);
	toollay->addWidget(colorFrame);
	toollay->addWidget(setColorBtn);
	toollay->addWidget(addImageBtn);
	//toollay->addWidget(addImageBackgroundBtn); //TBD
	toollay->addWidget(rectBtn);
	toollay->addWidget(circleBtn);
	toollay->addWidget(lineBtn);
	toollay->addWidget(clearBtn);

	QObject::connect(addImageBtn, SIGNAL(clicked()), this, SLOT(addImage()));
	//QObject::connect(addImageBackgroundBtn, SIGNAL(clicked()), this, SLOT(addImageBackground()));
	QObject::connect(setColorBtn, SIGNAL(clicked()), this, SLOT(setColorBtn_clicked()));
	QObject::connect(rectBtn, SIGNAL(clicked()), this, SLOT(rectBtn_clicked()));
	QObject::connect(circleBtn, SIGNAL(clicked()), this, SLOT(circleBtn_clicked()));
	QObject::connect(lineBtn, SIGNAL(clicked()), this, SLOT(lineBtn_clicked()));
	QObject::connect(clearBtn, SIGNAL(clicked()), this, SLOT(clearBtn_clicked()));
	QObject::connect(slider, SIGNAL(valueChanged(int)), this, SLOT(setPainterSize(int)));

}
void CNetPainter::addImage()
{
	view->drawDone = false;
	view->currentMode = drawMode::image;
	QString path = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Image Files(*.jpg *.png)"));
	view->brushImage.load(path);
}
void CNetPainter::addImageBackground()
{
	view->drawDone = false;
	QString path = QFileDialog::getOpenFileName(this, tr("Open Image"), ".", tr("Image Files(*.jpg *.png)"));
	QImage *im=new QImage(path);
	view->updateImage(*im);
}
void CNetPainter::setColorBtn_clicked()
{
	view->drawDone = false;
	c = QColorDialog::getColor(Qt::blue);
	colorFrame->setPalette(QPalette(c));
	view->color = c;
}
void CNetPainter::rectBtn_clicked()
{
	view->drawDone = false;
	view->currentMode = drawMode::rect;
}
void CNetPainter::circleBtn_clicked()
{
	view->drawDone = false;
	view->currentMode = drawMode::circle;
}
void CNetPainter::lineBtn_clicked()
{
	view->drawDone = false;
	view->currentMode = drawMode::line;
}
void CNetPainter::clearBtn_clicked()
{
	view->drawDone = false;
	view->currentMode = drawMode::clear;
}

void CNetPainter::setPainterSize(int value)
{
	view->drawDone = false;
	QString str = QString(qs("筆刷大小:%1px")).arg(value);
	sizeLabel->setText(str);
	view->painterSize = value;
}
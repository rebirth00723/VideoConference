#include "guiCPackage.h"

guiCPackage::guiCPackage(QWidget *parent)
:QFrame(parent)
{
	btnstylesheet = "QPushButton{	background - color:#000000; \
									color: #F8F8FF; \
									min-width: 6em;\
									border - radius: 10px;\
									border: 2px groove #CD6600; \
									border - style: outset; }"
		   "QPushButton:hover {		background-color:#F8F8FF; color: #000000;}";

	statusG = "background-color : #00FF00";
	statusR = "background-color : #FF0000";
}
void guiCPackage::newFrame(QFrame *frame, int id, QString mtitle, QString mtime, int mCase)
{
	status = new QLabel();
	status->setAutoFillBackground(true);
	status->setFixedWidth(10);
	nameTitle = new QLabel(qs("會議名稱:"));
	nameTitle->setFixedWidth(60);
	timeTitle = new QLabel(qs("會議時間:"));
	name = new QLabel;
	time = new QLabel;
	innbtn = new CPushButton();
	innbtn->setStyleSheet(btnstylesheet);

	gbox = new QGridLayout();
	gbox->addWidget(nameTitle, 0, 0, 1, 1);
	gbox->addWidget(name, 0, 1, 1, 1);
	gbox->addWidget(timeTitle, 1, 0, 1, 1);
	gbox->addWidget(time, 1, 1, 1, 1);

	hbox = new QHBoxLayout(frame);
	hbox->addWidget(status);
	hbox->addLayout(gbox);
	hbox->addWidget(innbtn);

	frame->setFixedSize(330, 60);
	frame->setFrameStyle(QFrame::Box | QFrame::DrawChildren);
	frame->setFont(QFont("", 10, QFont::Black));

	name->setText(mtitle);
	switch (mCase)
	{
	case 0:
		status->setStyleSheet(statusG);
		time->setText(mtime);
		break;
	case 1:
		status->setStyleSheet(statusR);
		time->setText(qs("尚未決定"));
		break;

	default:
		break;
	}
	innbtn->btnID = id;

	connect(innbtn, SIGNAL(clicked()), this, SLOT(onBtnClicked()));
}
void guiCPackage::onBtnClicked()
{
	CPushButton* buttonSender = qobject_cast<CPushButton*>(sender());

	emit showMID(buttonSender->btnID);
}

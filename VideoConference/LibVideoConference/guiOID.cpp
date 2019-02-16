#include "guiOID.h"

guiOID::guiOID(QWidget *parent)
: QDialog(parent)
{
	blackBackground.setColor(QPalette::Window, Qt::black);

	this->setWindowTitle(qs("使用資訊"));
	this->setFixedSize(260,350);
	this->setPalette(blackBackground);

	page1 = new QWidget();
	page2 = new QWidget();
	page3 = new QWidget();
	page4 = new QWidget();
	page5 = new QWidget();
	page6 = new QWidget();

	toolbox = new QToolBox();
	toolbox->setStyleSheet("background : #000000; color : #CD6600");
	toolbox->setFont(QFont("", 10, QFont::Black));
	toolbox->addItem(page1, qs("待開會議"));
	toolbox->addItem(page2, qs("未定會議"));
	toolbox->addItem(page3, qs("搜尋會議"));
	toolbox->addItem(page4, qs("待審會議"));
	toolbox->addItem(page5, qs("創建會議"));
	toolbox->addItem(page6, qs("歷史會議"));

	text1 = new QLabel();
	text2 = new QLabel();
	text3 = new QLabel();
	text4 = new QLabel();
	text5 = new QLabel();
	text6 = new QLabel();
	text1->adjustSize();
	text1->setWordWrap(true);
	text1->setAlignment(Qt::AlignTop);
	text1->setText(qs("即將要召開的會議，點擊會議資訊按鈕來查看更多會議資訊，或是加入會議"));
	text2->adjustSize();
	text2->setWordWrap(true);
	text2->setAlignment(Qt::AlignTop);
	text2->setText(qs("會議時間尚未決議的會議，正在等待其他會議參與者表決出會議時間，點擊會議資訊按鈕來查看更多會議資訊"));
	text3->adjustSize();
	text3->setWordWrap(true);
	text3->setAlignment(Qt::AlignTop);
	text3->setText(qs("當前的待開會議以及未定會議都可以在此搜尋，點擊會議資訊按鈕來查看更多會議資訊"));
	text4->adjustSize();
	text4->setWordWrap(true);
	text4->setAlignment(Qt::AlignTop);
	text4->setText(qs("尚未決定可出席時間的會議，請在投票期限內表決可出席的會議時間"));
	text5->adjustSize();
	text5->setWordWrap(true);
	text5->setAlignment(Qt::AlignTop);
	text5->setText(qs("創建一個會議，透過輸入會議名稱、會議說明、會議時間、參與者...等資訊來完成創建手續"));
	text6->adjustSize();
	text6->setWordWrap(true);
	text6->setAlignment(Qt::AlignTop);
	text6->setText(qs("查看已經完成會議的相關資訊"));

	textbox1 = new QVBoxLayout(page1);
	textbox1->addWidget(text1);
	textbox2 = new QVBoxLayout(page2);
	textbox2->addWidget(text2);
	textbox3 = new QVBoxLayout(page3);
	textbox3->addWidget(text3);
	textbox4 = new QVBoxLayout(page4);
	textbox4->addWidget(text4);
	textbox5 = new QVBoxLayout(page5);
	textbox5->addWidget(text5);
	textbox6 = new QVBoxLayout(page6);
	textbox6->addWidget(text6);

	vbox = new QVBoxLayout(this);
	vbox->setContentsMargins(30,20,30,20);
	vbox->addWidget(toolbox);
}
void guiOID::show_operatingInformation()
{
	this->show();
}
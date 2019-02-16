#include "guiOID.h"

guiOID::guiOID(QWidget *parent)
: QDialog(parent)
{
	blackBackground.setColor(QPalette::Window, Qt::black);

	this->setWindowTitle(qs("�ϥθ�T"));
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
	toolbox->addItem(page1, qs("�ݶ}�|ĳ"));
	toolbox->addItem(page2, qs("���w�|ĳ"));
	toolbox->addItem(page3, qs("�j�M�|ĳ"));
	toolbox->addItem(page4, qs("�ݼf�|ĳ"));
	toolbox->addItem(page5, qs("�Ыط|ĳ"));
	toolbox->addItem(page6, qs("���v�|ĳ"));

	text1 = new QLabel();
	text2 = new QLabel();
	text3 = new QLabel();
	text4 = new QLabel();
	text5 = new QLabel();
	text6 = new QLabel();
	text1->adjustSize();
	text1->setWordWrap(true);
	text1->setAlignment(Qt::AlignTop);
	text1->setText(qs("�Y�N�n�l�}���|ĳ�A�I���|ĳ��T���s�Ӭd�ݧ�h�|ĳ��T�A�άO�[�J�|ĳ"));
	text2->adjustSize();
	text2->setWordWrap(true);
	text2->setAlignment(Qt::AlignTop);
	text2->setText(qs("�|ĳ�ɶ��|���Mĳ���|ĳ�A���b���ݨ�L�|ĳ�ѻP�̪�M�X�|ĳ�ɶ��A�I���|ĳ��T���s�Ӭd�ݧ�h�|ĳ��T"));
	text3->adjustSize();
	text3->setWordWrap(true);
	text3->setAlignment(Qt::AlignTop);
	text3->setText(qs("��e���ݶ}�|ĳ�H�Υ��w�|ĳ���i�H�b���j�M�A�I���|ĳ��T���s�Ӭd�ݧ�h�|ĳ��T"));
	text4->adjustSize();
	text4->setWordWrap(true);
	text4->setAlignment(Qt::AlignTop);
	text4->setText(qs("�|���M�w�i�X�u�ɶ����|ĳ�A�Цb�벼��������M�i�X�u���|ĳ�ɶ�"));
	text5->adjustSize();
	text5->setWordWrap(true);
	text5->setAlignment(Qt::AlignTop);
	text5->setText(qs("�Ыؤ@�ӷ|ĳ�A�z�L��J�|ĳ�W�١B�|ĳ�����B�|ĳ�ɶ��B�ѻP��...����T�ӧ����Ыؤ���"));
	text6->adjustSize();
	text6->setWordWrap(true);
	text6->setAlignment(Qt::AlignTop);
	text6->setText(qs("�d�ݤw�g�����|ĳ��������T"));

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
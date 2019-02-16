#include "guiED.h"

guiED::guiED(QDialog *parent)
: QDialog(parent)
{
	colorDefine.setEDMIDColor(&blackBG, &orangeText, &btnStyleSheet);

	this->setWindowTitle(qs("系統訊息"));
	this->setFixedSize(300,100);
	this->setFont(QFont("", 10, QFont::Black));
	this->setPalette(blackBG);

	message = new QLabel;
	message->setPalette(orangeText); 
	message->setAlignment(Qt::AlignCenter);

	btn = new QPushButton;
	btn->setFixedHeight(30);
	btn->setStyleSheet(btnStyleSheet);

	signoutBtn = new QPushButton(qs("YES"));
	signoutBtn->setFixedHeight(30);
	signoutBtn->setEnabled(false);
	signoutBtn->hide();
	signoutBtn->setStyleSheet(btnStyleSheet);
	
	vbox = new QVBoxLayout(this);
	hbox = new QHBoxLayout();
	hSpacer = new QSpacerItem(150, 0, QSizePolicy::Minimum, QSizePolicy::Fixed);
	hbox->addItem(hSpacer);
	hbox->addWidget(signoutBtn);
	hbox->addWidget(btn);
	vbox->addWidget(message);
	vbox->addLayout(hbox);

	connect(btn, SIGNAL(clicked()), this, SLOT(close()));
	connect(signoutBtn, SIGNAL(clicked()), this, SLOT(signout()));
}
void guiED::EventDialog(int s, int err)
{
	btn->setText(qs("OK"));
	signoutBtn->setEnabled(false);
	signoutBtn->hide();
	std::string str;
	switch (s)
	{
	case 0:
		message->setText(qs("登入成功!"));
		this->exec();
		break;
	case 1:
		message->setText(qs("登入帳號不存在或密碼錯誤!"));
		this->exec();
		break;
	case 2:
		message->setText(qs("帳號密碼不得為空白!"));
		this->exec();
		break;
	case 3:
		message->setText(qs("登出成功!"));
		this->exec();
		break;
	case 4:
		message->setText(qs("請選擇可出席的會議時間!"));
		this->exec();
		break;
	case 5:
		message->setText(qs("已完成投票!"));
		this->exec();
		break;
	case 6:
		message->setText(qs("會議名稱不得空白!"));
		this->exec();
		break;
	case 7:
		message->setText(qs("缺少會議說明或附加說明檔案!"));
		this->exec();
		break;
	case 8:
		message->setText(qs("請新增會議時間選項!"));
		this->exec();
		break;
	case 9:
		message->setText(qs("請新增更多參加人員!"));
		this->exec();
		break;
	case 10:
		message->setText(qs("會議創建成功!"));
		this->exec();
		break;
	case 11:
		message->setText(qs("密碼修改成功!"));
		this->exec();
		break;
	case 12:
		message->setText(qs("新密碼不一致，請重新輸入!"));
		this->exec();
		break;
	case 13:
		message->setText(qs("舊密碼輸入錯誤!"));
		this->exec();
		break;
	case 14:
		message->setText(qs("無效的修改，請確認資料是否已填寫完成!"));
		this->exec();
		break;
	case 15:
		message->setText(qs("確定登出?"));
		btn->setText(qs("NO"));
		signoutBtn->show();
		signoutBtn->setEnabled(true);	
		this->exec();
		break;
	case 16:
		message->setText(qs("無法與伺服器連線，請檢察您的網路狀態!"));
		this->exec();
	case 17:
		str = "會議創建失敗!" + std::to_string(err);
		message->setText(QString::fromLocal8Bit(str.c_str()));
		this->exec();
		break;
	case 18:
		message->setText(qs("會議資料有錯誤，無法進行會議!"));
		this->exec();
		break;
	case 19:
		message->setText(qs("投票期限錯誤，請重新選填!"));
		this->exec();
		break;

	default:
		break;
	}
}

void guiED::signout()
{
	emit signout_Success();
	this->close();

}

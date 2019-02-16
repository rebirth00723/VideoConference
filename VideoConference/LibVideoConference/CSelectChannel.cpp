#include"CSelectChannel.h"

void CSelectChannel::updateList()
{
	cnt = 0;
	for (int i = 0; i < 100; i++)   //清空desklist
	{
		desklist[i] = 0;
	}
	for (int i = 0; i < 100; i++)
	{
		if ((idlistExPtr[i] == 1) && (memberlistExPtr->find(i) != memberlistExPtr->end()))
		{
			desklist[cnt] = i;
			cnt++;
		}
	}
	tablew->clear();   //清空頻道表格
	for (int i = 0; i <cnt; i++)
	{
		tablew->insertRow(i);

		QWidget *pWidget = new QWidget();
		QHBoxLayout *pLayout = new QHBoxLayout(pWidget);
		QCheckBox *pCheckBox = new QCheckBox();
		pLayout->setAlignment(Qt::AlignCenter);
		pLayout->setContentsMargins(0, 0, 0, 0);

		pLayout->addWidget(pCheckBox);
		pWidget->setLayout(pLayout);
		tablew->setCellWidget(i, 0, pWidget);

		int uid = desklist[i];
		auto it = memberlistExPtr->find(uid);
		if (it != memberlistExPtr->end())
		{
			QString name = QString::fromStdString(it->second.name.c_str());
			qDebug() << name << endl;
			tablew->setItem(i, 1, new QTableWidgetItem(name));
		}
	}
}
void CSelectChannel::okBtn_clicked()
{
	for (int i = 0; i < 100; i++)   //將已勾選項目歸零
	{
		checklist[i] = 0;
	}


	for (int i = 0; i <cnt; i++)
	{
		QWidget * widget = tablew->cellWidget(i, 0);   
	
		QCheckBox *checkbox = widget->findChild<QCheckBox *>();
	
		
		if (checkbox && checkbox->isChecked())
		{
			checklist[desklist[i]] = 1;

		}
	}
	for (int i = 0; i <100; i++)
	{
		cout << "ll" << checklist[2] << endl;
	}
	
	emit ok();
	this->hide();
}
CSelectChannel::CSelectChannel(int* idlist, std::map<int, CUserData> *memberlist){
	idlistExPtr = idlist;
	memberlistExPtr = memberlist;

	okBtn = new QPushButton(qs("確定"), this);
	vlay = new QVBoxLayout(this);

	tablew = new QTableWidget(this);
	tablew->setColumnCount(2);

	vlay->addWidget(tablew);
	vlay->addWidget(okBtn);
	QObject::connect(okBtn, SIGNAL(clicked()), this, SLOT(okBtn_clicked()));
}
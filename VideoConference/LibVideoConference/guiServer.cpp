#include "guiServer.h"
using namespace std;

guiServer::guiServer(QWidget *parent)
	: QWidget(parent)
{	
	setWindowTitle("Server");
	this->setFixedSize(730, 550);
	this->setFont(QFont("", 10, QFont::Black));

	page0 = new QWidget;
	page1 = new QWidget;
	page2 = new QWidget;
	tab = new QTabWidget;
	//tab->addTab(page0, "Conference Data");
	tab->addTab(page1, "User Data");
	tab->addTab(page2, "Department Data"); 

	add_Widget();
	add_Layout();
	page0_Widget();
	page0_Layout();
	page1_Widget();
	page1_Layout();
	page2_Widget();
	page2_Layout();
	totalLayout();
	
	/*新增會員*/
	connect(addUBtn, SIGNAL(clicked()), this, SLOT(addNewUser()));
	/*展開、關閉分頁*/
	connect(addBtn, SIGNAL(clicked()), this, SLOT(showAdd()));
	/*會員資料蒐尋*/
	connect(page1_searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(UserSearch()));
	connect(page1_searchBtn, SIGNAL(clicked()), this, SLOT(UserSearch()));
	/*會員資料修改*/
	connect(page1_editBtn, SIGNAL(clicked()), this, SLOT(editUser()));
	connect(page1_tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updateUserDate(QTableWidgetItem*)));
	connect(page1_tableWidget, SIGNAL(itemPressed(QTableWidgetItem*)), this, SLOT(setpwdBtnEnble(QTableWidgetItem*)));
	connect(page1_tableWidget, SIGNAL(itemPressed(QTableWidgetItem*)), this, SLOT(saveItemData(QTableWidgetItem*)));
	connect(page1_pwdBtn, SIGNAL(clicked()), this, SLOT(resetPassword()));
	/*新增部門*/
	connect(addDBtn, SIGNAL(clicked()), this, SLOT(addNewDepartment()));
	/*部門資料蒐尋*/
	connect(page2_searchLineEdit, SIGNAL(textChanged(QString)), this, SLOT(searchGroup()));
	connect(page2_searchBtn, SIGNAL(clicked()), this, SLOT(searchGroup()));
	/*部門資料修改*/
	connect(page2_editBtn, SIGNAL(clicked()), this, SLOT(editGroup()));
	connect(page2_deleteBtn, SIGNAL(clicked()), this, SLOT(deleteGroup()));
	connect(page2_tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(updataGroupDate(QTableWidgetItem*)));
	connect(page2_tableWidget, SIGNAL(itemPressed(QTableWidgetItem*)), this, SLOT(setDeleteBtnEnble(QTableWidgetItem*)));
	connect(page2_tableWidget, SIGNAL(itemPressed(QTableWidgetItem*)), this, SLOT(saveItemData(QTableWidgetItem*)));
	connect(&timer, SIGNAL(timeout()), this, SLOT(refreshData()));

	t = new QThread;
	timer.start(1000);
	timer.moveToThread(t);
	t->start();

}
QString guiServer::intToQString(int id)
{
	QString outputs = QString::number(id);
	return outputs;
}
QString guiServer::intToNumber(int num)
{
	QString outputs = QString("%1").arg(num, 0, 10, QChar('0'));
	return outputs;
}
QString guiServer::timeStampToQString(int timeStamp)
{
	QDateTime	time;
	QString		deadline;

	if (timeStamp == 63112320)
	{
		deadline = "NULL";
	}
	else
	{
		time = QDateTime::fromTime_t(timeStamp);
		deadline = time.toString("yyyy/MM/dd hh:mm");
	}
	return deadline;
}
QString guiServer::charToQString(char* c)
{
	QByteArray byte;
	byte = QByteArray(c);
	QString str;
	str = QString::fromLocal8Bit(byte);
	return str;
}
QString guiServer::intStatus(int status)
{
	QString	str = "";

	switch (status)
	{
	case 0:
		str = "Completed";
		break;

	case 1:
		str = "Uncompleted";
		break;
	default:
		break;
	}

	return str;
}


void guiServer::add_Widget()
{
	addBtn = new QPushButton("<<");
	addBtn->setFixedWidth(30);
	addUserTitle = new QLabel("Added New Users");
	addUserTitle->setFont(QFont("", 12, QFont::Black));
	userNameTitle = new QLabel("User Name:");
	userName = new QLineEdit;
	userPasswordTitle = new QLabel("User Password:");
	userPassword = new QLineEdit;
	userDepartmentTitle = new QLabel("User Department:(Input ID Number)");
	userDepartment = new QLineEdit;
	isCreatorTitle = new QLabel("Create permissions");
	isCreator = new QRadioButton("Yes");
	noCreator = new QRadioButton("No");
	noCreator->setChecked(true);
	addUBtn = new QPushButton("Send out");

	addDepartmentTitle = new QLabel("Added New Department");
	addDepartmentTitle->setFont(QFont("", 12, QFont::Black));
	departmentNameTitle = new QLabel("Department Name:");
	departmentName = new QLineEdit();
	addDBtn = new QPushButton("Send Out");

	hideAddWidget();
}
void guiServer::add_Layout()
{
	isCreatorHbox = new QHBoxLayout;
	isCreatorHbox->addWidget(isCreator);
	isCreatorHbox->addWidget(noCreator);


	addUserVbox = new QVBoxLayout;
	addUserVbox->addWidget(addUserTitle);
	addUserVbox->addWidget(userNameTitle);
	addUserVbox->addWidget(userName);
	addUserVbox->addWidget(userPasswordTitle);
	addUserVbox->addWidget(userPassword);
	addUserVbox->addWidget(userDepartmentTitle);
	addUserVbox->addWidget(userDepartment);
	addUserVbox->addWidget(isCreatorTitle);
	addUserVbox->addLayout(isCreatorHbox);
	addUserVbox->addWidget(addUBtn);
	hSpacer = new QSpacerItem(0, 400, QSizePolicy::Minimum, QSizePolicy::Fixed);
	addUserVbox->addItem(hSpacer);
	addUserVbox->addWidget(addDepartmentTitle);
	addUserVbox->addWidget(departmentNameTitle);
	addUserVbox->addWidget(departmentName);
	addUserVbox->addWidget(addDBtn);
}
void guiServer::hideAddWidget()
{
	addUserTitle->hide();
	userNameTitle->hide();
	userName->hide();
	userPasswordTitle->hide();
	userPassword->hide();
	userDepartmentTitle->hide();
	userDepartment->hide();
	isCreatorTitle->hide();
	isCreator->hide();
	noCreator->hide();
	addUBtn->hide();

	addDepartmentTitle->hide();
	departmentNameTitle->hide();
	departmentName->hide();
	addDBtn->hide();
}
void guiServer::showAddWidget()
{
	addUserTitle->show();
	userNameTitle->show();
	userName->show();
	userPasswordTitle->show();
	userPassword->show();
	userDepartmentTitle->show();
	userDepartment->show();
	isCreatorTitle->show();
	isCreator->show();
	noCreator->show();
	addUBtn->show();

	addDepartmentTitle->show();
	departmentNameTitle->show();
	departmentName->show();
	addDBtn->show();
}
void guiServer::page0_Widget()
{
	
	page0_tableWidget = new QTableWidget();
	page0_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);				//設定選擇行為，以行為單位
	page0_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);				//設定選擇模式，單行
	page0_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);				//設定為不可編輯
	page0_tableWidget->setColumnCount(6);
	page0_tableWidget->verticalHeader()->hide();
	QStringList tableHeader0;
	tableHeader0 <<"Conference ID"<< "Conference Name" << "Status" 
				<< "Starting Time"<< "End Time" <<"Participants";
	page0_tableWidget->setHorizontalHeaderLabels(tableHeader0);
	page0_tableWidget->setColumnWidth(0, 100);
	page0_tableWidget->setColumnWidth(1, 150);
	page0_tableWidget->setColumnWidth(2, 90);
	page0_tableWidget->setColumnWidth(3, 90);
	page0_tableWidget->setColumnWidth(4, 90);
	page0_tableWidget->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);

	page0_searchLineEdit = new QLineEdit();
	page0_searchLineEdit->setPlaceholderText("Search Conference");

	page0_searchComboBox = new QComboBox();
	page0_searchComboBox->insertItem(0, " ID");
	page0_searchComboBox->insertItem(1, " Name");

	page0_searchBtn = new QPushButton("Search!");

	page0_showStatus = new QLabel("Conference Status:");

	page0_allRB = new QRadioButton("All");
	page0_noStartedRB = new QRadioButton("Waiting");
	page0_noDecidedRB = new QRadioButton("Undecided");
	page0_historicalRB = new QRadioButton("Historical");
}
void guiServer::page0_Layout()
{
	page0_searchHbox = new QHBoxLayout();
	page0_searchHbox->addWidget(page0_searchLineEdit);
	page0_searchHbox->addWidget(page0_searchComboBox);
	page0_searchHbox->addWidget(page0_searchBtn);

	page0_rbHbox = new QHBoxLayout();
	page0_rbHbox->addWidget(page0_showStatus);
	page0_rbHbox->addWidget(page0_allRB);
	page0_rbHbox->addWidget(page0_noStartedRB);
	page0_rbHbox->addWidget(page0_noDecidedRB);
	page0_rbHbox->addWidget(page0_historicalRB);


	page0_Vbox = new QVBoxLayout(page0);
	page0_Vbox->addLayout(page0_searchHbox);
	page0_Vbox->addLayout(page0_rbHbox);
	page0_Vbox->addWidget(page0_tableWidget);
}
void guiServer::page1_Widget()
{
	page1_tableWidget = new QTableWidget;
	page1_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);				//設定選擇行為，以行為單位
	page1_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);				//設定選擇模式，單行
	page1_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);				//設定為不可編輯
	page1_tableWidget->setColumnCount(6);
	page1_tableWidget->verticalHeader()->hide();
	QStringList tableHeader1;
	tableHeader1 << "User ID" << "User Name"<< "Department ID"
				 <<"Creator"<<"Last Login"<<"Last Logout";
	page1_tableWidget->setHorizontalHeaderLabels(tableHeader1);
	page1_tableWidget->setColumnWidth(0, 100);
	page1_tableWidget->setColumnWidth(1, 120);
	page1_tableWidget->setColumnWidth(2, 100);
	page1_tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
	page1_tableWidget->setColumnWidth(4, 130);
	page1_tableWidget->setColumnWidth(5, 130);

	page1_searchLineEdit = new QLineEdit();
	page1_searchLineEdit->setPlaceholderText("Search User");

	page1_searchComboBox = new QComboBox();
	page1_searchComboBox->insertItem(0, "User ID");
	page1_searchComboBox->insertItem(1, "User Name");
	page1_searchComboBox->insertItem(2, "Department ID");

	page1_searchBtn = new QPushButton("Search!");

	page1_editBtn = new QPushButton("Edit user data");
	page1_pwdBtn = new QPushButton("Password initialization");
	page1_pwdBtn->setEnabled(false);
}
void guiServer::page1_Layout()
{
	page1_searchHbox = new QHBoxLayout;
	page1_searchHbox->addWidget(page1_searchLineEdit);
	page1_searchHbox->addWidget(page1_searchComboBox);
	page1_searchHbox->addWidget(page1_searchBtn);

	page1_editHBox = new QHBoxLayout;
	hSpacer = new QSpacerItem(500, 0, QSizePolicy::Minimum, QSizePolicy::Fixed);
	page1_editHBox->addItem(hSpacer);
	page1_editHBox->addWidget(page1_pwdBtn);
	page1_editHBox->addWidget(page1_editBtn);

	page1_Vbox = new QVBoxLayout(page1);
	page1_Vbox->addLayout(page1_searchHbox);
	page1_Vbox->addWidget(page1_tableWidget);
	page1_Vbox->addLayout(page1_editHBox);
}
void guiServer::page2_Widget()
{
	page2_tableWidget = new QTableWidget;
	page2_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);				//設定選擇行為，以行為單位
	page2_tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);				//設定選擇模式，單行
	page2_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);				//設定為不可編輯
	page2_tableWidget->setAlternatingRowColors(true);
	page2_tableWidget->setColumnCount(2);
	page2_tableWidget->verticalHeader()->hide();
	QStringList tableHeader2;
	tableHeader2 << "Department ID" << "Department Name";
	page2_tableWidget->setHorizontalHeaderLabels(tableHeader2);
	page2_tableWidget->setColumnWidth(0, 120);
	page2_tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

	page2_searchLineEdit = new QLineEdit();
	page2_searchLineEdit->setPlaceholderText("Search Department");

	page2_searchComboBox = new QComboBox();
	page2_searchComboBox->insertItem(0, " ID");
	page2_searchComboBox->insertItem(1, " Name");

	page2_searchBtn = new QPushButton("Search!");
	page2_editBtn = new QPushButton("Edit department data");
	page2_deleteBtn = new QPushButton("Delete department");
	page2_deleteBtn->setEnabled(false); 
}
void guiServer::page2_Layout()
{
	page2_searchHbox = new QHBoxLayout;
	page2_searchHbox->addWidget(page2_searchComboBox);
	page2_searchHbox->addWidget(page2_searchBtn);

	page2_LeftVbox = new QVBoxLayout;
	page2_LeftVbox->addWidget(page2_searchLineEdit);
	page2_LeftVbox->addLayout(page2_searchHbox);
	hSpacer = new QSpacerItem(0, 500, QSizePolicy::Minimum, QSizePolicy::Fixed);
	page2_LeftVbox->addItem(hSpacer);
	page2_LeftVbox->addWidget(page2_editBtn);
	page2_LeftVbox->addWidget(page2_deleteBtn);
	
	page2_HBox = new QHBoxLayout(page2);
	page2_HBox->addWidget(page2_tableWidget);
	page2_HBox->addLayout(page2_LeftVbox);
}
void guiServer::totalLayout()
{
	addBtnVbox = new QVBoxLayout();
	hSpacer = new QSpacerItem(0, 680, QSizePolicy::Minimum, QSizePolicy::Fixed);
	addBtnVbox->addWidget(addBtn);
	addBtnVbox->addItem(hSpacer);

	Hbox = new QHBoxLayout(this);
	Hbox->addLayout(addUserVbox);
	Hbox->addLayout(addBtnVbox);
	Hbox->addWidget(tab);
}

void guiServer::inputUserData(CUserData *data, int row)
{
	for (int i = 0; i < row; i++)
	{
		page1_tableWidget->insertRow(i);
		page1_tableWidget->setItem(i, 0, new QTableWidgetItem(intToNumber(data[i].id)));
		page1_tableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(data[i].name)));
		page1_tableWidget->setItem(i, 2, new QTableWidgetItem(intToNumber(data[i].groupid)));
		page1_tableWidget->setItem(i, 3, new QTableWidgetItem(intToQString(data[i].isCreator)));
	}
	
	/*page1_tableWidget->item(i, 0)->setTextAlignment(Qt::AlignCenter);
	page1_tableWidget->item(i, 2)->setTextAlignment(Qt::AlignCenter);
	page1_tableWidget->item(i, 3)->setTextAlignment(Qt::AlignCenter);*/
}
void guiServer::inputGroupData(CGroupData *data, int row)
{
	for (int i = 0; i < row; i++)
	{
		page2_tableWidget->insertRow(i);
		page2_tableWidget->setItem(i, 0, new QTableWidgetItem(intToNumber(data[i].id)));
		page2_tableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(data[i].name)));
	}
	
}
/*void guiServer::inputConferenceData(CUserData data, int i)
{
	page0_tableWidget->insertRow(i);
	page0_tableWidget->setItem(i, 0, new QTableWidgetItem(intToNumber(data.ID)));
	page0_tableWidget->setItem(i, 1, new QTableWidgetItem(charToQString(data.Name)));
	page0_tableWidget->setItem(i, 2, new QTableWidgetItem(intStatus(data.Status)));
	page0_tableWidget->setItem(i, 3, new QTableWidgetItem(timeStampToQString(data.StartingTime)));
	page0_tableWidget->setItem(i, 4, new QTableWidgetItem(timeStampToQString(data.EndTime)));
	page0_tableWidget->setItem(i, 5, new QTableWidgetItem(intToQString(data.Creator)));
	page0_tableWidget->setItem(i, 6, new QTableWidgetItem(intToQString(data.Participants)));
}*/
void guiServer::getAllData()
{
	CUserData_arr Udata;
	int row1 = sql.getUsersData(&Udata);
	inputUserData(Udata.get(), row1);

	CGroupData_arr Gdata;
	int row2 = sql.getGroups(&Gdata);
	inputGroupData(Gdata.get(), row2);
}
void guiServer::clearAllData()
{
	page1_tableWidget->setRowCount(0);
	page2_tableWidget->setRowCount(0);
	cout << "----- Clear all data. -----"<<endl;
}
void guiServer::refreshData()
{
	if (page1_updateLock == false && page2_updateLock == false)
	{
		clearAllData();
		getAllData();
	}	
}

void guiServer::showAdd()
{
	addFF = !addFF;

	if (addFF == true)
	{
		addBtn->setText(">>");
		showAddWidget();
	}
	else
	{
		addBtn->setText("<<");
		hideAddWidget();
	}

}
void guiServer::addNewUser()
{
	int sqlrt = SQL_ERROR;
	if (!userName->text().isEmpty() && !userPassword->text().isEmpty() && !userDepartment->text().isEmpty())
	{
		CUserData ud;
		bool ok;	

		ud.name = userName->text().toUtf8().data();
		ud.pwd = userPassword->text().toUtf8().data();
		ud.groupid = userDepartment->text().toInt(&ok,10);
		ud.isCreator = iscOrNoc();

		int sqlrt = sql.addUser(ud);

		if (sqlrt > 0)
		{
			userName->clear();
			userPassword->clear();
			userDepartment->clear();
			QMessageBox::information(this, "System message", "Created successfully!");
			
		}
		else if (sqlrt == SQL_REGISTER_USER_EXIST)
		{
			userName->clear();
			userPassword->clear();
			QMessageBox::information(this, "System message", "The user is already exist! Create failed!");
		}
		else if (sqlrt == SQL_REGISTER_GROUP_NOT_EXIST)
		{
			userDepartment->clear();
			QMessageBox::information(this, "System message", "Incorrect group! Create failed!");
		}
		
	}
	else
		QMessageBox::information(this, "System message", "Field should not be blank!");

}
void guiServer::addNewDepartment()
{
	int sqlrt = SQL_ERROR;
	if (!departmentName->text().isEmpty())
	{
		std::string str = departmentName->text().toUtf8().constData();	
		int sqlrt = sql.addGroup(str);

		if (sqlrt == SQL_SUCCESS)
		{
			QMessageBox::information(this, "System message", "Created successfully!");
		}
		else if (sqlrt == SQL_ADDGROUP_GROUP_EXIST)
		{
			QMessageBox::information(this, "System message", "The department is already exist! Create failed!");
		}
		departmentName->clear();
	}
	else
		QMessageBox::information(this, "System message", "Field should not be blank!");
}

void guiServer::UserSearch()
{
	QString str = page1_searchLineEdit->text();
	int index = page1_searchComboBox->currentIndex();

	if (str.isEmpty())
	{
		page1_updateLock = false;
		for (int i = 0; i < page1_tableWidget->rowCount(); i++)
		{
			page1_tableWidget->showRow(i);
		}
	}
	else
	{
		page1_updateLock = true;
		for (int row = 0; row < page1_tableWidget->rowCount(); row++)
			page1_tableWidget->hideRow(row);


		switch (index)
		{
		case 0:
			for (int i = 0; i < page1_tableWidget->rowCount(); i++)
			{
				QTableWidgetItem *item = page1_tableWidget->item(i, 0);
				if (item->text().contains(str))
				{
					page1_tableWidget->showRow(i);
				}
			}
			break;

		case 1:
			for (int i = 0; i < page1_tableWidget->rowCount(); i++)
			{
				QTableWidgetItem *item = page1_tableWidget->item(i, 1);
				if (item->text().contains(str))
				{
					page1_tableWidget->showRow(i);
				}
			}
			break;

		case 2:
			for (int i = 0; i < page1_tableWidget->rowCount(); i++)
			{
				QTableWidgetItem *item = page1_tableWidget->item(i, 2);
				if (item->text().contains(str))
				{
					page1_tableWidget->showRow(i);
				}
			}
			break;

		default:
			break;
		}
	}
}
void guiServer::editUser()
{
	qDebug() << "----------------------";
	
	page1_EditLock = !page1_EditLock;
	if (page1_EditLock == true)
	{
		page1_editBtn->setText("Edit completed");
		userEditMode();
		page1_updateLock = true;
		page1_pwdLock = true;
	}
	else
	{
		page1_editBtn->setText("Edit  user data");
		page1_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
		page1_updateLock = false;
		page1_pwdLock = false;
		page1_pwdBtn->setEnabled(false);
	}
}
void guiServer::userEditMode()
{
	page1_tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
	for (int i = 0; i < page1_tableWidget->rowCount(); i++)
	{
		page1_tableWidget->item(i, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		/*page1_tableWidget->item(i, 4)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		page1_tableWidget->item(i, 5)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);*/
	}
}
void guiServer::updateUserDate(QTableWidgetItem *item)
{
	if (item->column() >= 1 && page1_updateLock == true)
	{
		bool ok;
		QString uids = page1_tableWidget->item(item->row(), 0)->text();

		QString	editData;
		int id;
		std::string name;
		int group;
		int creator;
		int sqlrt;

		switch (item->column())
		{
		case 1:
			editData = page1_tableWidget->item(item->row(), item->column())->text();
			qDebug() << "Edit: user id:" << uids << " new user name:" << editData;

			id = uids.toInt(&ok, 10);
			name = editData.toUtf8().data();

			sqlrt = sql.updateUserName(id, name);
			break;

		case 2:
			editData = page1_tableWidget->item(item->row(), item->column())->text();
			qDebug() << "Edit: user id:" << uids << " new user group:" << editData;

			id = uids.toInt(&ok, 10);
			group = editData.toInt(&ok, 10);

			sqlrt = sql.updateUserGroup(id, group);
			break;

		case 3:
			editData = page1_tableWidget->item(item->row(), item->column())->text();
			qDebug() << "Edit: user id:" << uids << " set user creator:" << editData;

			id = uids.toInt(&ok, 10);
			creator = editData.toInt(&ok, 10);

			sqlrt = sql.updateUserCreator(id, creator);
			break;
		default:
			break;
		}

		switch (sqlrt)
		{
		case SQL_UPDATE_GROUP_NOT_EXIST:
			QMessageBox::information(this, "System message", "Update user data failed! Incorrect group.");
			item->setText(Register);
			break;
		case SQL_UPDATE_INCORRECT_CREATOR:
			QMessageBox::information(this, "System message", "Update user data failed! Incorrect creator.");
			item->setText(Register);
			break;

		case SQL_ERROR:
			QMessageBox::information(this, "System message", "SQL error.");
			item->setText(Register);
			break;
		default:
			break;
		}
	}
	
}
void guiServer::setpwdBtnEnble(QTableWidgetItem *item)
{
	if (item->column() == 0 && page1_pwdLock == true)
	{
		//qDebug() << "pwdBtn enable";
		page1_pwdBtn->setEnabled(true);
	}
	else 
	{
		//qDebug() << "pwdBtn disable";
		page1_pwdBtn->setEnabled(false);
	}
}
void guiServer::resetPassword()
{
	bool ok;
	int id;
	std::string pwd= "0000000";	
	int sqlrt;

	QTableWidgetItem *item = page1_tableWidget->currentItem();

	id = item->text().toInt(&ok,10);
	sqlrt = sql.updateUserPwd(id, pwd);

	switch (sqlrt)
	{
	case SQL_SUCCESS:
		QMessageBox::information(this, "System message", QString("User ID: %1 ""Reset password successfully").arg(id));
		break;

	default:
		QMessageBox::information(this, "System message", "SQL error");
		break;
	}
}

void guiServer::searchGroup()
{
	QString str = page2_searchLineEdit->text();
	int index = page2_searchComboBox->currentIndex();

	if (str.isEmpty())
	{
		page2_updateLock = false;
		for (int i = 0; i < page2_tableWidget->rowCount(); i++)
		{
			page2_tableWidget->showRow(i);
		}
	}
	else
	{
		page2_updateLock = true;
		for (int row = 0; row < page2_tableWidget->rowCount(); row++)
			page2_tableWidget->hideRow(row);


		switch (index)
		{
		case 0:
			for (int i = 0; i < page2_tableWidget->rowCount(); i++)
			{
				QTableWidgetItem *item = page2_tableWidget->item(i, 0);
				if (item->text().contains(str))
				{
					page2_tableWidget->showRow(i);
				}
			}
			break;

		case 1:
			for (int i = 0; i < page2_tableWidget->rowCount(); i++)
			{
				QTableWidgetItem *item = page2_tableWidget->item(i, 1);
				if (item->text().contains(str))
				{
					page2_tableWidget->showRow(i);
				}
			}
			break;

		default:
			break;
		}
	}
}
void guiServer::editGroup()
{
	qDebug() << "----------------------";
	
	page2_EditLock = !page2_EditLock;
	if (page2_EditLock == true)
	{
		page2_editBtn->setText("Edit completed");
		groupEditMode();
		page2_updateLock = true;
		page2_delteLock = true;
	}
	else
	{
		page2_editBtn->setText("Edit department name");
		page2_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
		page2_deleteBtn->setEnabled(false);
		page2_updateLock = false;
		page2_delteLock = false;
	}
}
void guiServer::deleteGroup()
{
	bool ok;
	int rowIndex = page2_tableWidget->currentRow();
	QTableWidgetItem *item = page2_tableWidget->item(rowIndex, 0);
	int groupID = item->text().toInt(&ok, 10);
	
	if (rowIndex >= 0)
	{
		int sqlrt = sql.deleteGroup(groupID);
		switch (sqlrt)
		{
		case SQL_SUCCESS:
			page2_tableWidget->removeRow(rowIndex);
			QMessageBox::information(this, "System message", QString("Delete department(ID: %1) successfully!").arg(groupID));
			break;
		case SQL_ERROR:
			QMessageBox::information(this, "System message", QString("Delete failed! The Department(ID: %1)  has other users!").arg(groupID));
			break;
		default:
			break;
		}
	}
	page2_deleteBtn->setEnabled(false);
	
}
void guiServer::updataGroupDate(QTableWidgetItem* item)
{
	bool ok;
	int sqlrt;
	if (item->column() >= 1 && page2_updateLock == true)
	{
		QString gids = page2_tableWidget->item(item->row(),0)->text();
		QString gnames = page2_tableWidget->item(item->row(), item->column())->text();
		qDebug() <<"Edit group id:" << gids <<" new group name:"<< gnames;

		int gid = gids.toInt(&ok,10);
		std::string gname = gnames.toUtf8().data();
		sqlrt = sql.updateGroupData(gid, gname);
		switch (sqlrt)
		{
		case SQL_UPDATE_GROUP_EXIST:
				QMessageBox::information(this, "System message", "Update group failed! Exist same name of group.");
				item->setText(Register);
				break;
				
		default:
			break;
		}
	}
	
}
void guiServer::groupEditMode()
{
	page2_tableWidget->setEditTriggers(QAbstractItemView::AllEditTriggers);
	for (int i = 0; i < page2_tableWidget->rowCount(); i++)
	{
		page2_tableWidget->item(i, 0)->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	}
}
void guiServer::setDeleteBtnEnble(QTableWidgetItem* item)
{
	if (item->column() == 0 && page2_delteLock == true)
	{
		//qDebug() << "pwdBtn enable";
		page2_deleteBtn->setEnabled(true);
	}
	else
	{
		//qDebug() << "pwdBtn disable";
		page2_deleteBtn->setEnabled(false);
	}
}

int guiServer::iscOrNoc()
{
	if (isCreator->isChecked())
		return  1;
	else
		return  0;
}
void guiServer::saveItemData(QTableWidgetItem* item)
{
	Register = item->text();
}
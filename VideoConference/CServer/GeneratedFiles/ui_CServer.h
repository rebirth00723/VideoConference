/********************************************************************************
** Form generated from reading UI file 'CServer.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CSERVER_H
#define UI_CSERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CServerClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *CServerClass)
    {
        if (CServerClass->objectName().isEmpty())
            CServerClass->setObjectName(QStringLiteral("CServerClass"));
        CServerClass->resize(600, 400);
        menuBar = new QMenuBar(CServerClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        CServerClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(CServerClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        CServerClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(CServerClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        CServerClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(CServerClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        CServerClass->setStatusBar(statusBar);

        retranslateUi(CServerClass);

        QMetaObject::connectSlotsByName(CServerClass);
    } // setupUi

    void retranslateUi(QMainWindow *CServerClass)
    {
        CServerClass->setWindowTitle(QApplication::translate("CServerClass", "CServer", 0));
    } // retranslateUi

};

namespace Ui {
    class CServerClass: public Ui_CServerClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CSERVER_H

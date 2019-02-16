/********************************************************************************
** Form generated from reading UI file 'LibVideoConference.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LIBVIDEOCONFERENCE_H
#define UI_LIBVIDEOCONFERENCE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LibVideoConferenceClass
{
public:

    void setupUi(QWidget *LibVideoConferenceClass)
    {
        if (LibVideoConferenceClass->objectName().isEmpty())
            LibVideoConferenceClass->setObjectName(QStringLiteral("LibVideoConferenceClass"));
        LibVideoConferenceClass->resize(600, 400);

        retranslateUi(LibVideoConferenceClass);

        QMetaObject::connectSlotsByName(LibVideoConferenceClass);
    } // setupUi

    void retranslateUi(QWidget *LibVideoConferenceClass)
    {
        LibVideoConferenceClass->setWindowTitle(QApplication::translate("LibVideoConferenceClass", "LibVideoConference", 0));
    } // retranslateUi

};

namespace Ui {
    class LibVideoConferenceClass: public Ui_LibVideoConferenceClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LIBVIDEOCONFERENCE_H

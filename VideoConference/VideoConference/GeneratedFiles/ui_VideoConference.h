/********************************************************************************
** Form generated from reading UI file 'VideoConference.ui'
**
** Created by: Qt User Interface Compiler version 5.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIDEOCONFERENCE_H
#define UI_VIDEOCONFERENCE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_VideoConferenceClass
{
public:

    void setupUi(QWidget *VideoConferenceClass)
    {
        if (VideoConferenceClass->objectName().isEmpty())
            VideoConferenceClass->setObjectName(QStringLiteral("VideoConferenceClass"));
        VideoConferenceClass->resize(600, 400);

        retranslateUi(VideoConferenceClass);

        QMetaObject::connectSlotsByName(VideoConferenceClass);
    } // setupUi

    void retranslateUi(QWidget *VideoConferenceClass)
    {
        VideoConferenceClass->setWindowTitle(QApplication::translate("VideoConferenceClass", "VideoConference", 0));
    } // retranslateUi

};

namespace Ui {
    class VideoConferenceClass: public Ui_VideoConferenceClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIDEOCONFERENCE_H

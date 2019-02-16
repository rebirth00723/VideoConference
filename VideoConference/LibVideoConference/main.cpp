#include "LibVideoConference.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LibVideoConference w;
    w.show();
    return a.exec();
}

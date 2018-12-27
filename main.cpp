#include "loserial.h"
#include <QApplication>

int main(int argc, char *argv[])
{
#ifdef Q_OS_MAC
    QCoreApplication::setOrganizationDomain("cn.long");
#endif
    QCoreApplication::setOrganizationName("long");
    QCoreApplication::setApplicationName("LoSerial");

    QApplication a(argc, argv);
    LoSerial w;
    w.show();
    return a.exec();
}

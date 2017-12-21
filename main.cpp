#include "loserial.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    LoSerial w;
    w.show();

    return a.exec();
}

#include <QtGui/QApplication>
#include "qtincoming.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtIncoming w;
    w.show();
    return a.exec();
}

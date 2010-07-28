#include <QtGui/QApplication>
#include "qtincoming.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << "before creation";
    QtIncoming w;
    qDebug() << "after creation";
    w.show();
    qDebug() << "should be shown";
    return a.exec();
}

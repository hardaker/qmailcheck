#include <QtGui/QApplication>
#include <QtGui/QStyleFactory>
#include "qtincoming.h"
#include <qdebug.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (a.style()->inherits("QMaemo5Style")) // provides miserable small screen scrolling
        a.setStyle("Cleanlooks");
    QtIncoming w;
    w.show();
    return a.exec();
}

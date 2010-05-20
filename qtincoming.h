#ifndef QTINCOMING_H
#define QTINCOMING_H

#include <QMainWindow>

namespace Ui {
    class QtIncoming;
}

class QtIncoming : public QMainWindow {
    Q_OBJECT
public:
    QtIncoming(QWidget *parent = 0);
    ~QtIncoming();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::QtIncoming *ui;
};

#endif // QTINCOMING_H

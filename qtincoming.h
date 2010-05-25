#ifndef QTINCOMING_H
#define QTINCOMING_H

#include <QMainWindow>
#include "ui_prefs.h"
#include "incomingmailmodel.h"

namespace Ui {
    class QtIncoming;
}

class QtIncoming : public QMainWindow {
    Q_OBJECT
public:
    QtIncoming(QWidget *parent = 0);
    ~QtIncoming();


public slots:
    void maybeRaise();
    void showPrefs();
    void changedSettings();
    void cancelled();
    void saveSettings();
    void readSettings();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::QtIncoming    *ui;
    Ui::PrefWindow    *prefui;
    QDialog           *prefDialog;
    IncomingMailModel *mailModel;
    bool               do_popup;
};

#endif // QTINCOMING_H

#ifndef QTINCOMING_H
#define QTINCOMING_H

#include <QMainWindow>
#include "ui_prefs.h"
#include "incomingmailmodel.h"
#include "foldermodel.h"

namespace Ui {
    class QtIncoming;
    class PrefWindow;
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
    void sendNotification(QString message);
    void fontButton();

    void set_highlightNew(bool);
    void set_doNotification(bool);
    void set_doPopup(bool);

protected:
    void changeEvent(QEvent *e);

private:
    QTableView        *mailView;
    Ui::QtIncoming    *ui;
    Ui::PrefWindow    *prefui;
    QDialog           *prefDialog;
    IncomingMailModel *mailModel;
    folderModel       *folderListModel;
    bool               do_popup;
    bool               m_doNotification;
    bool               m_notifyCritical;
    bool               m_highlightNew;
};

#endif // QTINCOMING_H

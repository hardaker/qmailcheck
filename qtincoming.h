#ifndef QTINCOMING_H
#define QTINCOMING_H

#include <QMainWindow>
#include <QtDBus/QDBusInterface>

#include "ui_prefs.h"
#include "incomingmailmodel.h"
#include "foldermodel.h"
#include <QApplication>

#if defined(Q_WS_MAEMO_5) || defined(MAEMO_CHANGES)
#define IS_MAEMO 1
#endif

namespace Ui {
    class QtIncoming;
    class PrefWindow;
}

class QtIncoming : public QMainWindow {
    Q_OBJECT
public:
    QtIncoming(QWidget *parent = 0, QApplication *app = 0);
    ~QtIncoming();


public slots:
    void maybeRaise();
    void showPrefs();
    void changedSettings();
    void cancelled();
    void saveSettings();
    void readSettings();
    void sendNotification(QString message, bool folderNotification);
    void fontButton();
    void newMail();

    void set_highlightNew(bool);
    void set_doNotification(bool);
    void set_doPopup(bool);

    void doPopup();
    void doNotification();
    void doLED();
    void doVirbrate();
    void stopVibrate();
    void clearLED();

    void switchStyle(const QString &newstyle);

protected:
    void changeEvent(QEvent *e);
    void setupStyleMenu();

private:
    QApplication      *m_app;
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
    bool               m_firstCheck;
    QDBusInterface    *m_dbusInterface;

};

#endif // QTINCOMING_H

#include <libnotify/notify.h>

#include "qtincoming.h"
#include "ui_qtincoming.h"
#include "ui_prefs.h"
#include "incomingmailmodel.h"
#include "foldermodel.h"
#include <QSettings>

#include <iostream>
#define DEBUG(x) std::cerr << x;

QtIncoming::QtIncoming(QWidget *parent) :
    QMainWindow(parent),
    mailView(0),
    ui(new Ui::QtIncoming), prefui(new Ui::PrefWindow),
    prefDialog(new QDialog()),
    do_popup(true)
{
    const char name[] = "qmailcheck";

    notify_init(name);

    ui->setupUi(this);

    mailModel = new IncomingMailModel(this);
    mailView = ui->mailTable;
    mailView->setModel(mailModel);
    mailView->setWordWrap(false);
    for(int i = 0; i < 20; i++) {
        mailView->setRowHeight(i, 1);
    }
    mailView->resizeColumnsToContents();
    mailView->resizeRowsToContents();

    folderListModel = new folderModel(this);
    mailModel->set_folderList(folderListModel);

    // signals from the mailbox
    connect(mailModel, SIGNAL(mailUpdated()),
            mailView, SLOT(resizeRowsToContents()));
    connect(mailModel, SIGNAL(mailUpdated()),
            mailView, SLOT(resizeColumnsToContents()));

    connect(mailModel, SIGNAL(updateCount(int, int)),
            mailView, SLOT(rowCountChanged(int, int)));
    connect(mailModel, SIGNAL(statusMessage(const QString &, int)),
            ui->statusBar, SLOT(showMessage(const QString &, int)));
    connect(mailModel, SIGNAL(newMail()),
            this, SLOT(maybeRaise()));
    connect(mailModel, SIGNAL(mailUpdated()),
            mailView, SLOT(repaint()));

    // signals from the buttons
    connect(ui->checkMail, SIGNAL(clicked()),
            mailModel, SLOT(checkMail()));
    connect(ui->acknowledge, SIGNAL(clicked()),
            mailModel, SLOT(clearNew()));
    connect(ui->preferences, SIGNAL(clicked()),
            this, SLOT(showPrefs()));

    connect(ui->hideAll, SIGNAL(clicked()),
            mailModel, SLOT(hideMessages()));
    connect(ui->unhideAll, SIGNAL(clicked()),
            mailModel, SLOT(clearHideList()));

    connect(ui->actionPreferences, SIGNAL(activated()),
            this, SLOT(showPrefs()));

    // setup the preferences UI
    prefui->setupUi(prefDialog);
    readSettings();
    QTableView *view = prefui->folderList;
    view->setModel(folderListModel);
    //prefDialog->show();
    connect(prefui->buttonBox, SIGNAL(accepted()),
            this, SLOT(changedSettings()));
    connect(prefui->buttonBox, SIGNAL(rejected()),
            this, SLOT(cancelled()));

    // don't do popups for the first mail check
    mailModel->checkMail();

    // connect this after the initial check mail pass
    connect(mailModel, SIGNAL(newMailMessage(QString)),
            this, SLOT(sendNotification(QString)));
}

QtIncoming::~QtIncoming()
{
    delete ui;
}

void QtIncoming::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void QtIncoming::sendNotification(QString message)
{
    const char name[] = "qmailcheck";
    NotifyNotification *notification;

    if (!do_popup)
        return;

    notification = notify_notification_new(name, message.toAscii().data(),
                                           NULL, NULL);
    if (notification) {
        // notify_notification_set_timeout(notification, 3000);
        if (!notify_notification_show(notification, NULL))
            DEBUG("Failed to send notification");
        
        if (m_notifyCritical)
            notify_notification_set_urgency(notification,
                                            NOTIFY_URGENCY_CRITICAL);

        /* Clean up the memory */
        g_object_unref(notification);
    } else {
        DEBUG("failed to create notification");
    }
}

void QtIncoming::maybeRaise()
{
    if (do_popup) {
        show();
        raise();
    }
}

void QtIncoming::showPrefs()
{
    if (prefDialog)
        prefDialog->show();
    
}

void QtIncoming::changedSettings()
{
    DEBUG("settings changed!\n");
    saveSettings();
    readSettings();
    mailModel->checkMail();
    this->repaint();
//    prefDialog = 0;
}

void QtIncoming::cancelled() 
{
    DEBUG("settings cancelled\n");
    // prefDialog = 0;
}

void QtIncoming::saveSettings()
{
    QSettings settings("Wes Hardaker", "qmailcheck");
    settings.setValue("popupWindow", prefui->popupWindow->isChecked());
    settings.setValue("notifyCritical", prefui->criticalNotifications->isChecked());
    settings.setValue("checkInterval", prefui->checkMail->text());
    settings.setValue("serverName", prefui->serverName->text());
    settings.setValue("serverPort", prefui->serverPort->text());
    settings.setValue("userName", prefui->userName->text());
    settings.setValue("password", prefui->password->text());
    folderListModel->saveSettings(settings);
}

void QtIncoming::readSettings()
{
    QSettings settings("Wes Hardaker", "qmailcheck");
    if (settings.value("popupWindow").toBool()) {
        prefui->popupWindow->setCheckState( Qt::Checked );
        do_popup = true;
    } else {
        prefui->popupWindow->setCheckState( Qt::Unchecked );
        do_popup = false;
    }

    if (settings.value("notifyCritical").toBool()) {
        prefui->criticalNotifications->setCheckState( Qt::Checked );
        m_notifyCritical = true;
    } else {
        prefui->criticalNotifications->setCheckState( Qt::Unchecked );
        m_notifyCritical = false;
    }

    prefui->checkMail->setText(settings.value("checkInterval").toString());
    mailModel->set_checkinterval(settings.value("checkInterval").toInt());

    prefui->serverName->setText(settings.value("serverName").toString());
    mailModel->set_hostname(QString(settings.value("serverName").toString()));

    prefui->password->setText(settings.value("password").toString());
    mailModel->set_password(settings.value("password").toString());

    prefui->userName->setText(settings.value("username").toString());
    mailModel->set_username(settings.value("username").toString());

    prefui->serverPort->setText(settings.value("serverPort").toString());
    mailModel->set_portnumber(settings.value("serverPort").toInt());

    folderListModel->readSettings(settings);

    mailModel->reInitializeSocket();
}



#include "moc_qtincoming.cpp"

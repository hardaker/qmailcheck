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
    ui(new Ui::QtIncoming), prefui(new Ui::PrefWindow), prefDialog(0),
    do_popup(true)
{
    ui->setupUi(this);

    mailModel = new IncomingMailModel(this);
    QTableView *view = ui->mailTable;
    view->setModel(mailModel);

    folderListModel = new folderModel(this);
    mailModel->set_folderList(folderListModel);

    connect(mailModel, SIGNAL(mailUpdated()),
            view, SLOT(resizeRowsToContents()));
    connect(mailModel, SIGNAL(mailUpdated()),
            view, SLOT(resizeColumnsToContents()));
    connect(mailModel, SIGNAL(updateCount(int, int)),
            view, SLOT(rowCountChanged(int, int)));
    view->resizeColumnsToContents();
    view->resizeRowsToContents();

    connect(mailModel, SIGNAL(statusMessage(const QString &, int)),
            ui->statusBar, SLOT(showMessage(const QString &, int)));
    connect(ui->checkMail, SIGNAL(clicked()),
            mailModel, SLOT(checkMail()));
    connect(mailModel, SIGNAL(newMail()),
            this, SLOT(maybeRaise()));

    connect(ui->actionPreferences, SIGNAL(activated()),
            this, SLOT(showPrefs()));

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

void QtIncoming::maybeRaise()
{
    if (do_popup)
        raise();
}

void QtIncoming::showPrefs()
{
    if (prefDialog)
        prefDialog->show();
    
    prefui->setupUi(prefDialog = new QDialog());
    readSettings();
    QTableView *view = prefui->folderList;
    view->setModel(folderListModel);
    prefDialog->show();
    connect(prefui->buttonBox, SIGNAL(accepted()),
            this, SLOT(changedSettings()));
    connect(prefui->buttonBox, SIGNAL(rejected()),
            this, SLOT(cancelled()));
}

void QtIncoming::changedSettings()
{
    DEBUG("settings changed!\n");
    saveSettings();
    readSettings();
    prefDialog = 0;
}

void QtIncoming::cancelled() 
{
    DEBUG("settings cancelled\n");
    prefDialog = 0;
}

void QtIncoming::saveSettings()
{
    QSettings settings("Wes Hardaker", "qmailcheck");
    settings.setValue("popupWindow", prefui->popupWindow->isChecked());
    settings.setValue("checkInterval", prefui->checkMail->text());
    settings.setValue("serverName", prefui->serverName->text());
    settings.setValue("serverPort", prefui->serverPort->text());
    settings.setValue("userName", prefui->userName->text());
    settings.setValue("password", prefui->password->text());
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

    mailModel->reInitializeSocket();
}



#include "moc_qtincoming.cpp"

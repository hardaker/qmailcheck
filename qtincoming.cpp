#include <libnotify/notify.h>

#include "qtincoming.h"
#include "ui_qtincoming.h"
#include "ui_prefs.h"
#include "incomingmailmodel.h"
#include "foldermodel.h"
#include <QSettings>
#include <QFontDialog>
#include <QtGui/QPushButton>
#include <QtGui/QLineEdit>

#if IS_MAEMO
#include <mce/dbus-names.h>
#endif

#include "qmailcheckcommon.h"

QtIncoming::QtIncoming(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QtIncoming), prefui(new Ui::PrefWindow),
    prefDialog(new QDialog(parent, Qt::Window)),
    do_popup(true), m_doNotification(true), m_highlightNew(true), m_firstCheck(true)
{
    const char name[] = "qmailcheck";

    notify_init(name);

    ui->setupUi(this);

    mailView = ui->mailTable;
    mailModel = new IncomingMailModel(0, this, mailView);
    mailView->setModel(mailModel);
    mailView->setWordWrap(false);
    for(int i = 0; i < 20; i++) {
        mailView->setRowHeight(i, 1);
    }
    mailView->resizeColumnsToContents();
    mailView->resizeRowsToContents();

    mailView->setShowGrid(false);
    //mailView->horizontalHeader()->hide();
    mailView->verticalHeader()->hide();
    mailView->horizontalHeader()->setMinimumSectionSize(1);
    mailView->verticalHeader()->setMinimumSectionSize(1);

    folderListModel = new folderModel(this, prefui);
    mailModel->set_folderList(folderListModel);

    mailModel->connectSignals(mailView, this);

#ifdef USE_STATUSBAR
    connect(mailModel, SIGNAL(statusMessage(const QString &, int)),
            ui->statusBar, SLOT(showMessage(const QString &, int)));
#endif

    // signals from the buttons
    connect(ui->checkMail, SIGNAL(clicked()),
            mailModel, SLOT(checkMailSlot()));
    connect(ui->acknowledge, SIGNAL(clicked()),
            mailModel, SLOT(clearNew()));
    connect(ui->acknowledge, SIGNAL(clicked()),
            this, SLOT(clearLED()));
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
#if defined(Q_WS_MAEMO_5) || defined(MAEMO_CHANGES)
    prefDialog->setAttribute((Qt::WidgetAttribute) 127); // Qt::WA_Maemo5StackedWindow
#endif
    prefDialog->setSizeGripEnabled(true);
    readSettings();

    // preference dialog
    connect(prefui->buttonBox, SIGNAL(accepted()),
            this, SLOT(changedSettings()));
    connect(prefui->buttonBox, SIGNAL(accepted()),
            folderListModel, SLOT(changedSettings()));
    connect(prefui->buttonBox, SIGNAL(rejected()),
            this, SLOT(cancelled()));
    connect(prefui->fontSelectButton, SIGNAL(clicked()),
            this, SLOT(fontButton()));

    connect(prefui->tabWidget, SIGNAL(currentChanged(int)), folderListModel, SLOT(setupFolderPrefs(int)));

#ifdef IS_MAEMO
    m_dbusInterface = new QDBusInterface(MCE_SERVICE, MCE_REQUEST_PATH, MCE_REQUEST_IF, QDBusConnection::systemBus(), this);
    QDBusMessage reply = m_dbusInterface->call(MCE_ENABLE_LED);
    if (reply.type() == QDBusMessage::ErrorMessage)
        qDebug() << reply.errorMessage();

    // enable the vibrator
    m_dbusInterface->call(MCE_ENABLE_VIBRATOR);
#endif

    mailModel->restartCheckers();
}


void QtIncoming::newMail() {
    mailView->resizeRowsToContents();
    mailView->resizeColumnsToContents();
    if (m_firstCheck) {
        mailModel->clearNew();
        m_firstCheck = false;
    }
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

void QtIncoming::sendNotification(QString message, bool folderNotification)
{
    const char name[] = "qmailcheck";
    NotifyNotification *notification;

    if (m_firstCheck || !m_doNotification || !folderNotification)
        return;

    notification = notify_notification_new(name, message.toAscii().data(),
                                           NULL, NULL);
    if (notification) {
        notify_notification_set_timeout(notification, 60000);
        if (!notify_notification_show(notification, NULL)) {
            DEBUG("Failed to send notification");
        }
        
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
    folderListModel->setupFolderPrefs(prefui->tabWidget->currentIndex());
}

void QtIncoming::fontButton() {
    bool ok;
    QFont font = QFontDialog::getFont(&ok, mailModel->font(), this);
    if (ok) {
        mailModel->set_font(font);
    }
}


void QtIncoming::changedSettings()
{
    DEBUG("settings changed!\n");
    saveSettings();
    readSettings();
    this->repaint();
//    prefDialog = 0;
}

void QtIncoming::cancelled() 
{
    DEBUG("settings cancelled\n");
    // prefDialog = 0;
}

void QtIncoming::set_doPopup(bool newval)
{
    do_popup = newval;
}

void QtIncoming::set_highlightNew(bool newval)
{
    m_highlightNew = newval;
}

void QtIncoming::set_doNotification(bool newval)
{
    m_doNotification = newval;
}

void QtIncoming::saveSettings()
{
    QSettings settings("Wes Hardaker", "qmailcheck");
    settings.setValue("popupWindow", prefui->popupWindow->isChecked());
    settings.setValue("doNotification", prefui->doNotification->isChecked());
    settings.setValue("notifyCritical", prefui->criticalNotifications->isChecked());
    settings.setValue("highlightNew", prefui->highlightNew->isChecked());
    settings.setValue("checkInterval", prefui->checkMail->text());
    mailModel->saveSettings(settings, prefui);
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

    if (settings.value("doNotification").toBool()) {
        prefui->doNotification->setCheckState( Qt::Checked );
        m_doNotification = true;
    } else {
        prefui->doNotification->setCheckState( Qt::Unchecked );
        m_doNotification = false;
    }

    if (settings.value("notifyCritical").toBool()) {
        prefui->criticalNotifications->setCheckState( Qt::Checked );
        m_notifyCritical = true;
    } else {
        prefui->criticalNotifications->setCheckState( Qt::Unchecked );
        m_notifyCritical = false;
    }

    if (settings.value("highlightNew").toBool()) {
        prefui->highlightNew->setCheckState( Qt::Checked );
        m_highlightNew = true;
    } else {
        prefui->highlightNew->setCheckState( Qt::Unchecked );
        m_highlightNew = false;
    }
    mailModel->set_highlightNew(m_highlightNew);

    prefui->checkMail->setText(settings.value("checkInterval").toString());
    mailModel->set_checkinterval(settings.value("checkInterval").toInt());

    mailModel->readSettings(settings, prefui);

    folderListModel->readSettings(settings);

    mailModel->restartCheckers();
}

void QtIncoming::doLED() {
    if (m_firstCheck)
        return;
#ifdef IS_MAEMO
        QDBusMessage reply;

        qDebug() << "setting LED";

        // set the LED pattern
        reply = m_dbusInterface->call(MCE_ACTIVATE_LED_PATTERN, "PatternError");
        if (reply.type() == QDBusMessage::ErrorMessage)
            qDebug() << reply.errorMessage();

        // turn on the display too
        reply = m_dbusInterface->call(MCE_DISPLAY_ON_REQ, 1);
        if (reply.type() == QDBusMessage::ErrorMessage)
            qDebug() << reply.errorMessage();
#endif
}

void QtIncoming::clearLED() {
#ifdef IS_MAEMO
    QDBusMessage reply;
    reply = m_dbusInterface->call(MCE_DEACTIVATE_LED_PATTERN, "PatternError");
    if (reply.type() == QDBusMessage::ErrorMessage)
        qDebug() << reply.errorMessage();
#endif
}

void QtIncoming::doNotification() {

}

void QtIncoming::doPopup() {
    maybeRaise();
}

void QtIncoming::doVirbrate() {
    if (m_firstCheck)
        return;
#ifdef IS_MAEMO
    qDebug() << "vibrating";
    m_dbusInterface->call(MCE_ACTIVATE_VIBRATOR_PATTERN, "PatternChatAndEmail");
    QTimer::singleShot(1000, this, SLOT(stopVibrate()));
#endif
}

void QtIncoming::stopVibrate() {
#ifdef IS_MAEMO
    m_dbusInterface->call(MCE_DEACTIVATE_VIBRATOR_PATTERN, "PatternChatAndEmail");
#endif
}

#include "moc_qtincoming.cpp"

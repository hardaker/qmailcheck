#ifndef MAILCHECKER_H
#define MAILCHECKER_H

#include <QThread>
#include <QMutex>
#include <QtNetwork/QSslSocket>
#include "foldermodel.h"

#include "incomingmailmodel.h"

class MailChecker : public QThread
{
    Q_OBJECT
public:
    MailChecker(IncomingMailModel *model, QMutex *mutex, MailSource *mailSource, folderModel *folderModel, int checkInterval,
                QList<MailMsg> *messages);
    void run();
    void shutDown();
    void initializeSocket();
    void reInitializeSocket();
    QList<QString> sendCommand(const QString &cmd);
    void connectSignals(QTableView *mailView, QtIncoming *mainWidget);

signals:
    void mailUpdated();
    void newMail();
    void updateCount(int from, int to);
    void newMailMessage(const QString newmessage, bool folderNotifies);
    void statusMessage(const QString &message, int timeout);
    void internalCheckMailSignal();

    void popupNotification();
    void LEDNotification();
    void notificationNotification();
    void vibrateNotification();

public slots:
    void checkMail();
    void internalCheckMail();

private:
    QSslSocket             *m_socket;
    QTimer                 *m_timer;
    IncomingMailModel      *m_model;
    QMutex                 *m_mutex;
    int                     __counter;
    MailSource             *m_mailSource;
    folderModel            *m_folderModel;
    QList<MailMsg>         *m_messages;
    QString                 m_statusMessage;
    bool                    m_checkingNow;
};

#endif // MAILCHECKER_H

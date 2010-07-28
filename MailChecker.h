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
    void setupTimer();
    void connectSignals(QTableView *mailView, QtIncoming *mainWidget);

signals:
    void mailUpdated();
    void newMail();
    void updateCount(int from, int to);
    void newMailMessage(const QString newmessage);
    void statusMessage(const QString &message, int timeout);

public slots:
    void checkMail();

private:
    QSslSocket             *m_socket;
    IncomingMailModel      *m_model;
    QMutex                 *m_mutex;
    QTimer                  m_timer;
    int                     __counter;
    MailSource             *m_mailSource;
    folderModel            *m_folderModel;
    int                     m_checkInterval;
    QList<MailMsg>         *m_messages;
    QString                 m_statusMessage;
};

#endif // MAILCHECKER_H

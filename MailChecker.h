#ifndef MAILCHECKER_H
#define MAILCHECKER_H

#include <QThread>
#include <QMutex>

#include "incomingmailmodel.h"

class MailChecker : public QThread
{
    Q_OBJECT
public:
    explicit MailChecker(IncomingMailModel *model, QMutex *mutex, QObject *parent = 0);
    void run();
    void shutDown();
    void initializeSocket();
    void reInitializeSocket();
    QList<QString> sendCommand(const QString &cmd);
    void checkMail();
    void setupTimer();

signals:
    void mailUpdated();
    void newMail();
    void updateCount(int from, int to);
    void newMailMessage(const QString newmessage);
    void statusMessage(const QString &message, int timeout);

public slots:

private:
    IncomingMailModel      *m_model;
    QMutex                 *m_mutex;
    QTimer                  m_timer;
    int                     __counter;
};

#endif // MAILCHECKER_H

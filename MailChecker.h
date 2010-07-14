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
    void checkMail();


signals:

public slots:

private:
    IncomingMailModel      *m_model;
    QMutex                 *m_mutex;
};

#endif // MAILCHECKER_H

#include "MailChecker.h"

MailChecker::MailChecker(IncomingMailModel *model, QMutex *mutex, QObject *parent) :
    QThread(parent), m_model(model), m_mutex(mutex)
{
}

void MailChecker::run() {

}

void MailChecker::shutDown() {

}



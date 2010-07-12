#include "MailSource.h"

MailSource::MailSource(QObject *parent) :
    QObject(parent)
{
}

MailSource::MailSource(QObject *parent, const QString &mailServer, const QString &userName, const QString &passPhrase, bool ignoreCertErrors)
    : QObject(parent), m_mailServer(mailServer), m_userName(userName), m_passPhrase(passPhrase), m_ignoreCertErrors(ignoreCertErrors)
{
}

const QString &MailSource::mailServer() const {
    return m_mailServer;
}

const QString &MailSource::userName() const {
    return m_userName;
}

const QString &MailSource::passPhrase() const {
    return m_passPhrase;
}

bool MailSource::ignoreCertErrors() const {
    return m_ignoreCertErrors;
}

void MailSource::set_mailServer(const QString &mailServer) {
    m_mailServer = mailServer;
}

void MailSource::set_userName(const QString &userName) {
    m_userName = userName;
}

void MailSource::set_passPhrase(const QString &passPhrase) {
    m_passPhrase = passPhrase;
}

void MailSource::set_ignoreCerterrors(bool ignoreErrors) {
    m_ignoreCertErrors = ignoreErrors;
}

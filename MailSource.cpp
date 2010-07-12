#include "MailSource.h"

MailSource::MailSource(QObject *parent) :
    QObject(parent)
{
}

MailSource::MailSource(QObject *parent, const QString &hostName, int portNumber, const QString &userName, const QString &passPhrase, bool ignoreCertErrors)
    : QObject(parent), m_hostName(hostName), m_portNumber(portNumber), m_userName(userName), m_passPhrase(passPhrase), m_ignoreCertErrors(ignoreCertErrors)
{
}

const QString &MailSource::hostName() const {
    return m_hostName;
}

int MailSource::portNumber() const {
    return m_portNumber;
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

void MailSource::set_hostName(const QString &hostName) {
    m_hostName = hostName;
}

void MailSource::set_portNumber(int portNumber) {
    m_portNumber = portNumber;
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

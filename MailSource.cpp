#include <QtGui/QMessageBox>
#include <QtGui/QInputDialog>
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

const QString &MailSource::noSavePassPhrase() const {
    return m_noSavePassPhrase;
}

QString MailSource::openPassPhrase(const QString &loginDescription) {
    if (!m_noSavePassPhrase.isEmpty())
        return m_noSavePassPhrase;
    if (!m_passPhrase.isEmpty())
        return m_passPhrase;

    // We don't have a pass phrase yet, so prompt for one and then save it
    QMessageBox passphraseDialog();
    bool pressedOk;
    QString text = QInputDialog::getText(0, loginDescription,
                                              tr("Passphrase for %1:").arg(loginDescription), QLineEdit::Password,
                                              tr(""), &pressedOk);
    if (pressedOk) {
        m_noSavePassPhrase = text;
        return text;
    }
    return QString();
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

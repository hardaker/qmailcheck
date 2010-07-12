#ifndef MAILSOURCE_H
#define MAILSOURCE_H

#include <QObject>

class MailSource : public QObject
{
    Q_OBJECT
public:
    explicit MailSource(QObject *parent = 0);
    MailSource(QObject *parent, const QString &mailServer, const QString &userName, const QString &passPhrase, bool ignoreCertErrors = false);

    const QString &mailServer() const;
    const QString &userName() const;
    const QString &passPhrase() const;
    bool          ignoreCertErrors() const;

    void    set_mailServer(const QString &mailServer);
    void    set_userName(const QString &userName);
    void    set_passPhrase(const QString &passPhrase);
    void    set_ignoreCerterrors(bool ignoreErrors);

signals:

public slots:

private:
    QString     m_mailServer;
    QString     m_userName;
    QString     m_passPhrase;
    bool        m_ignoreCertErrors;

};

#endif // MAILSOURCE_H

#ifndef MAILSOURCE_H
#define MAILSOURCE_H

#include <QObject>

class MailSource : public QObject
{
    Q_OBJECT
public:
    explicit MailSource(QObject *parent = 0);
    MailSource(QObject *parent, const QString &hostName, int portNumber, const QString &userName, const QString &passPhrase, bool ignoreCertErrors = false);

    const QString &hostName() const;
    int           portNumber() const;
    const QString &userName() const;
    const QString &passPhrase() const;
    const QString &noSavePassPhrase() const;
    QString openPassPhrase(const QString &loginDescription);
    bool          ignoreCertErrors() const;

    void    set_hostName(const QString &hostName);
    void    set_portNumber(int portNumber);
    void    set_userName(const QString &userName);
    void    set_passPhrase(const QString &passPhrase);
    void    set_ignoreCerterrors(bool ignoreErrors);
signals:

public slots:

private:
    QString     m_hostName;
    int         m_portNumber;
    QString     m_userName;
    QString     m_passPhrase;
    QString     m_noSavePassPhrase;
    bool        m_ignoreCertErrors;

};

#endif // MAILSOURCE_H

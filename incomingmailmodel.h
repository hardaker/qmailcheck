#ifndef INCOMINGMAILMODEL_H
#define INCOMINGMAILMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QtNetwork/QSslSocket>

#include "mailmsg.h"

class IncomingMailModel : public QAbstractTableModel
{
Q_OBJECT
public:
    explicit IncomingMailModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    void initializeSocket();
    void checkMail();
    QList<QString> sendCommand(const QString &cmd);

signals:

public slots:

private:
    QStringList mailInfo;
    QSslSocket m_socket;
    int __counter;
    QStringList mailBoxes;

    QList<MailMsg> m_messages;

    QString m_username;
    QString m_password;
    QString m_hostname;
    int m_port;
    
};

#endif // INCOMINGMAILMODEL_H

#ifndef INCOMINGMAILMODEL_H
#define INCOMINGMAILMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QStringList>
#include <QtNetwork/QSslSocket>
#include <QtCore/QTimer>

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
    void setupTimer();
    QList<QString> sendCommand(const QString &cmd);

signals:
    void mailUpdated();

public slots:
    void checkMail();
    void gotUpdated();

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
    
    QTimer m_timer;
};

#endif // INCOMINGMAILMODEL_H

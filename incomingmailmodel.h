#ifndef INCOMINGMAILMODEL_H
#define INCOMINGMAILMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QStringList>
#include <QtNetwork/QSslSocket>
#include <QtCore/QTimer>
#include <QRegExp>
#include <QtGui/QFont>

#include "mailmsg.h"
#include "foldermodel.h"
#include "ui_prefs.h"
#include "MailSource.h"

class MailChecker;
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
    QFont font();

    void initializeSocket();
    void reInitializeSocket();

    void setupTimer();
    QList<QString> sendCommand(const QString &cmd);

    void set_checkinterval(int interval);
    void set_highlightNew(bool newval);
    void set_font(const QFont &font);

    void set_folderList(folderModel *list);
    void emitChanges();
    void readSettings(QSettings &settings, Ui::PrefWindow *prefui);
    void saveSettings(QSettings &settings, Ui::PrefWindow *prefui);

signals:
    void mailUpdated();
    void newMail();
    void updateCount(int from, int to);
    void statusMessage(const QString &message, int timeout);
    void newMailMessage(const QString newmessage);

public slots:
    void checkMail();
    void clearNew();
    void hideMessages();
    void clearHideList();

private:
    friend class MailChecker;

    QSslSocket m_socket;
    int __counter;
    folderModel *folderList;

    QList<MailMsg> m_messages;
    QList<QString> m_hideList;

    QList<MailSource *> m_mailSources;
    
    int m_checkinterval;
    bool m_highlightNew;

    QString m_statusMessage;
    QFont   m_font;
};

#endif // INCOMINGMAILMODEL_H

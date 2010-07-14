#ifndef INCOMINGMAILMODEL_H
#define INCOMINGMAILMODEL_H

#include <QObject>
#include <QAbstractTableModel>
#include <QStringList>
#include <QtNetwork/QSslSocket>
#include <QtCore/QTimer>
#include <QRegExp>
#include <QtGui/QFont>
#include <QtCore/QMutex>

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
    void newMail();

public slots:
    void clearNew();
    void hideMessages();
    void clearHideList();
    void changedSettings();
    void restartCheckers();

private:
    friend class MailChecker;

    MailChecker *m_checker;
    QMutex      *m_mutex;

    QSslSocket m_socket;
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

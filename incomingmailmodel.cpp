#include "incomingmailmodel.h"

#include <QtGui/QLabel>
#include <QtNetwork/QSslSocket>

#define DATE_WIDTH    5
#define FROM_WIDTH    20
#define SUBJECT_WIDTH 48

// #define DEBUG(x) std::cerr << x;
#include <iostream>
#define OUTPUT(x) std::cerr << x;
#define DEBUG(x) OUTPUT(x)

enum column_list 
{
    COL_FOLDER  = 0,
    COL_DATE    = 1,
    COL_FROM    = 2,
    COL_SUBJECT = 3
};

IncomingMailModel::IncomingMailModel(QObject *parent) :
    QAbstractTableModel(parent), m_socket(),
    __counter(0), folderList(0), m_messages(), m_hideList(),
    m_username(), m_password(), m_hostname(),
    m_portnumber(993), m_timer(), m_checkinterval(600), m_statusMessage(), m_highlightNew(true)
{
}

int IncomingMailModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_messages.length();
    //return 20;
}

int IncomingMailModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

Qt::ItemFlags IncomingMailModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    
    return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled;
}

QVariant IncomingMailModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QList<MailMsg>::const_iterator message;
    QList<MailMsg>::const_iterator stopat = m_messages.end();
    int findrow = index.row();
    QString lastfolder;
    
    for(message = m_messages.begin(); message != stopat; ++message) {
        if (! m_hideList.contains(message->uid()))
            findrow--;
        if (findrow == -1)
            break;
        if (message == stopat)
            return QVariant();
        lastfolder = message->folder();
    }

    if (message == stopat)
        return QVariant();

    if (role == Qt::BackgroundRole && index.column() < 4) {
        // QColor(128,255,255);n
        // return QApplication::palette().colorGroup(QPalette::Highlight);
        if (message->isnew())
#if defined(Q_WS_MAEMO_5) || defined(MAEMO_CHANGES)
            return QColor(255,255,255);
#else
            return QApplication::palette().highlight();
#endif
        else
            return QVariant();
    }

    if (role == Qt::ForegroundRole && index.column() < 4) {
        if (message->isnew())
#if defined(Q_WS_MAEMO_5) || defined(MAEMO_CHANGES)
            return QColor(128,128,255);
#else
            return QApplication::palette().highlightedText();
#endif
        else
            return QVariant();
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.column() > 4)
        return QVariant();

    switch(index.column()) {

    case COL_FOLDER:
        if (message->folder() == lastfolder)
            return QVariant();
        return message->folder();

    case COL_DATE:
        return message->time();

    case COL_FROM:
        return message->from();

    case COL_SUBJECT:
        return message->subject();

    default:
        return QString("x");
        
    }
}

QVariant IncomingMailModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    
#if 0
    if (role == Qt::SizeHintRole)
        return QSize(1,1);
#endif
    return QVariant();
#if 0
    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg("section");
    else
        return QString("Row %1").arg("section");
#endif
}

void
IncomingMailModel::reInitializeSocket()
{
    if (!m_socket.isOpen())
        m_socket.close();
    initializeSocket();
}

        

void
IncomingMailModel::initializeSocket()
{
    if (!m_socket.isOpen() && m_username != "" && m_password != "" &&
        m_hostname != "" && m_portnumber != 0) {

        DEBUG("Opening Connection\n");

        m_socket.setPeerVerifyMode(QSslSocket::VerifyNone);
        m_socket.connectToHostEncrypted(m_hostname, m_portnumber);
        m_socket.waitForReadyRead();

        // read throw-away line info
        char buf[4096];
        m_socket.readLine( buf, sizeof( buf ) );

        DEBUG("intial line: " << buf);

        // XXX: should check for error
        sendCommand(QString("login ") + m_username + " " + m_password);
        // XXX: A1 NO [AUTHENTICATIONFAILED] Authentication failed.

        setupTimer();
    }
}

void
IncomingMailModel::checkMail()
{
    
    DEBUG("Attempting to check mail...\n");

    if (!folderList || folderList->count() == 0) {
        if (!folderList) {
            DEBUG("no folder list\n");
        } else {
            DEBUG("folder list empty\n");
        }
        return;
    }

    if (! m_socket.isOpen()) {
        DEBUG("socket not open\n");
        initializeSocket();
        if (! m_socket.isOpen())
            return;
    }

    QMap<QString, bool> uid_list;
    for(int i = 0; i < m_messages.count(); i++) {
        uid_list.insert(m_messages[i].uid(), m_messages[i].isnew());
    }

    int oldcount = m_messages.count();
    m_messages.clear();

    QList<QString> results;
    QRegExp subjectMatch("Subject: +(.*)");
    QRegExp fromMatch("From: +(.*)");
    QRegExp dateMatch("Date: +(.*)");
    bool containsNewMessages = false;

    for(int mbox = 0; mbox < folderList->count(); mbox++) {

        if (folderList->folderName(mbox) == "")
            continue;

        //DEBUG("---- " << folderList->folderName(mbox).toAscii().data() << " ------\n");

        sendCommand(QString("EXAMINE \"" + folderList->folderName(mbox) + "\""));

        results = sendCommand(QString("UID SEARCH RECENT"));
        QStringList msglist = results[0].split(' ');
        for(int i = 2; i < msglist.length(); i++) {
            QString subject, from, date;
            QStringList headers =
                sendCommand(QString("UID FETCH " + msglist[i] +
                                    " (FLAGS BODY[HEADER])"));
            for(int j = 0; j < headers.length(); j++) {
                if (subjectMatch.indexIn(headers[j]) != -1) {
                    subject = subjectMatch.cap( 1 );
                } else if (fromMatch.indexIn(headers[j]) != -1) {
                    from = fromMatch.cap( 1 );
                } else if (dateMatch.indexIn(headers[j]) != -1) {
                    date = dateMatch.cap( 1 );
                }
            }

            MailMsg message(msglist[i], folderList->folderName(mbox), date, from, subject);
            if (m_highlightNew && ! uid_list.contains(msglist[i])) {
                message.setIsNew(true);
                containsNewMessages = true;
                emit newMailMessage(from + "\n" + subject);
            } else {
                message.setIsNew(uid_list[msglist[i]]);
            }
            
            m_messages.push_back(message);

            date.truncate(DATE_WIDTH);
            from.truncate(FROM_WIDTH);
            subject.truncate(SUBJECT_WIDTH);
            QString output = QString("%1 | %2 | %3")
                .arg(date, - DATE_WIDTH)
                .arg(from, - FROM_WIDTH)
                .arg(subject, - SUBJECT_WIDTH);
            //DEBUG(output.toAscii().data() << "\n");
        }
    }
    emit mailUpdated();
    emit updateCount(oldcount, m_messages.count());
    if (containsNewMessages) {
        emit newMail();
        emitChanges();
    }
    m_statusMessage = QString("%1 messages available").arg(m_messages.count());
    emit statusMessage(m_statusMessage, 0);
}

QList<QString>
IncomingMailModel::sendCommand(const QString &cmd) {
    char buf[1024];

    QString fullcmd ( QString('A') + QString::number(++__counter) +
                      QString(' ') + cmd + QString('\n') );
    //DEBUG ("sending: " << fullcmd.toAscii().data());
    m_socket.write(fullcmd.toAscii().data(), fullcmd.length());

    QRegExp donematch(QString("^A") + QString::number(__counter) + QString(" "));

    bool notDone = true;
    QList<QString> results;
    while(notDone) {
        if ( m_socket.bytesAvailable() <= 0 )
            m_socket.waitForReadyRead();

        int linelength = m_socket.readLine( buf, sizeof( buf ) );
        if (linelength <= 0) {
            exit(42); // XXX
        }

        QString resultString(buf);
        resultString = resultString.trimmed();
        results.push_back(resultString);
        // DEBUG( "data: " << resultString.toAscii().data() << "\n");

        if (donematch.indexIn(resultString) != -1) {
            notDone = false;
            // DEBUG( "end marker found " << donematch.pattern().toAscii().data() << "\n");
        }
    }
    //DEBUG( "----\n");
    return results;
}

void
IncomingMailModel::setupTimer()
{
    m_timer.stop();
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(checkMail()));
    m_timer.start(m_checkinterval * 1000);
}

void IncomingMailModel::set_hostname(QString hostname) {
    m_hostname = hostname;
}

void IncomingMailModel::set_username(QString username) {
    m_username = username;
}

void IncomingMailModel::set_password(QString password) {
    m_password = password;
}

void IncomingMailModel::set_portnumber(int portnumber) {
    m_portnumber = portnumber;
}

void IncomingMailModel::set_checkinterval(int checkinterval) {
    m_checkinterval = checkinterval;
}

void IncomingMailModel::set_folderList(folderModel *list) 
{
    folderList = list;
}

void IncomingMailModel::set_highlightNew(bool newval)
{
    m_highlightNew = newval;
}

void IncomingMailModel::clearHideList()
{
    m_hideList.clear();
    emitChanges();
}

void IncomingMailModel::hideMessages()
{
    QList<MailMsg>::iterator stopAt = m_messages.end();
    QList<MailMsg>::iterator message;
    for(message = m_messages.begin(); message != stopAt; ++message) {
        if (! m_hideList.contains(message->uid()))
            m_hideList.push_back(message->uid());
    }
    emitChanges();
}

void IncomingMailModel::clearNew()
{
    for(int i = 0; i < m_messages.count(); i++) {
        m_messages[i].setIsNew(false);
    }
    emitChanges();
}

void IncomingMailModel::emitChanges()
{
    emit dataChanged(createIndex(0,0,0),
                     createIndex(m_messages.count(),3,0));
    emit layoutChanged();
}

#include "moc_incomingmailmodel.cpp"

#include "incomingmailmodel.h"

#include <QtGui/QLabel>
#include <QtNetwork/QSslSocket>

#define DATE_WIDTH    5
#define FROM_WIDTH    20
#define SUBJECT_WIDTH 48

#include "qmailcheckcommon.h"

enum column_list 
{
    COL_FOLDER  = 0,
    COL_DATE    = 1,
    COL_FROM    = 2,
    COL_SUBJECT = 3
};

IncomingMailModel::IncomingMailModel(QObject *parent) :
    QAbstractTableModel(parent), m_socket(),
    __counter(0), folderList(0), m_messages(), m_hideList(), m_timer(), m_checkinterval(600), m_statusMessage(), m_highlightNew(true)
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

    if (index.column() > COL_SUBJECT)
        return QVariant();

    if (role == Qt::FontRole) {
        return m_font;
    }

    if (false && Qt::SizeHintRole == role){
        return (QVariant(m_font.pointSize()));
        QVariant size = QAbstractTableModel::data(index, Qt::SizeHintRole);
        if (size.toInt() > 5)
            return QVariant(size.toInt() - 5);
        else
            return size;
    }

    if (role == Qt::BackgroundRole) {
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

    if (role == Qt::ForegroundRole) {
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
    if (m_socket.isOpen())
        m_socket.close();
    initializeSocket();
}

void IncomingMailModel::readSettings(QSettings &settings, Ui::PrefWindow *prefui) {
    if (m_mailSources.count() == 0) {
        m_mailSources.append(new MailSource());
    }

    QList<MailSource *>::iterator source = m_mailSources.begin();

    prefui->serverName->setText(settings.value("serverName").toString());
    (*source)->set_hostName(QString(settings.value("serverName", "localhost").toString()));

    prefui->password->setText(settings.value("password").toString());
    (*source)->set_passPhrase(settings.value("password", "").toString());

    prefui->userName->setText(settings.value("username").toString());
    (*source)->set_userName(settings.value("username", "").toString());

    prefui->serverPort->setText(settings.value("serverPort").toString());
    (*source)->set_portNumber(settings.value("serverPort", 993).toInt());

    set_font(settings.value("font").value<QFont>());
}

void IncomingMailModel::saveSettings(QSettings &settings, Ui::PrefWindow *prefui) {
    settings.setValue("serverName", prefui->serverName->text());
    settings.setValue("serverPort", prefui->serverPort->text());
    settings.setValue("userName", prefui->userName->text());
    settings.setValue("password", prefui->password->text());
    settings.setValue("font", m_font);
}

void IncomingMailModel::initializeSocket()
{
    if (!m_socket.isOpen() && m_mailSources.count() > 0) {
        QList<MailSource *>::iterator     source;
        QList<MailSource *>::iterator     sourceEnd = m_mailSources.end();

        for(source = m_mailSources.begin(); source != sourceEnd; ++source) {

            DEBUG("Opening Connection\n");
            qDebug() << (*source)->hostName() << "/" << (*source)->portNumber();

            if (true || (*source)->ignoreCertErrors())
                m_socket.setPeerVerifyMode(QSslSocket::VerifyNone);
            m_socket.connectToHostEncrypted((*source)->hostName(), (*source)->portNumber());
            m_socket.waitForReadyRead();

            // read throw-away line info
            char buf[4096];
            m_socket.readLine( buf, sizeof( buf ) );

            DEBUG("intial line: " << buf);

            // XXX: should check for error
            sendCommand(QString("login ") + (*source)->userName() + " " + (*source)->passPhrase());
            // XXX: A1 NO [AUTHENTICATIONFAILED] Authentication failed.

            setupTimer();
        }
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
        if (results.length() == 0) {
            // socket probably died.
            reInitializeSocket();
            return;
        }
            
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
            reInitializeSocket();
            return results; // XXX: need to throw an error or something
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

void IncomingMailModel::set_font(const QFont &font) {
    m_font = font;
}

QFont IncomingMailModel::font() {
    return m_font;
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

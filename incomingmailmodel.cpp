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

IncomingMailModel::IncomingMailModel(QObject *parent) :
    QAbstractTableModel(parent), mailInfo(), m_socket(),
    __counter(0),
    m_username(), m_password(), m_hostname(),
    m_portnumber(993), m_timer(), m_checkinterval(600), m_statusMessage()
{
    mailInfo.push_back("foo");

    mailBoxes.push_back("imap.inbox");
    mailBoxes.push_back("imap.tislabs");

    initializeSocket();
    setupTimer();
    checkMail();
}

int IncomingMailModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 20;
    //return m_messages.length();
}

int IncomingMailModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

Qt::ItemFlags IncomingMailModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return Qt::ItemIsSelectable | Qt::ItemIsEnabled;
}

QVariant IncomingMailModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
        return QVariant();

    if (index.row() >= m_messages.length())
        return QVariant();

    if (index.column() > 4)
        return QVariant();

    switch(index.column()) {

    case 0:
        return m_messages[index.row()].folder();

    case 1:
        return m_messages[index.row()].time();

    case 2:
        return m_messages[index.row()].from();

    case 3:
        return m_messages[index.row()].subject();

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
        int linelength = m_socket.readLine( buf, sizeof( buf ) );

        DEBUG("intial line: " << buf);

        // XXX: should check for error
        sendCommand(QString("login ") + m_username + " " + m_password);
        // XXX: A1 NO [AUTHENTICATIONFAILED] Authentication failed.
    }
}

void
IncomingMailModel::checkMail()
{
    
    if (! m_socket.isOpen()) {
        initializeSocket();
        if (! m_socket.isOpen())
            return;
    }

    int oldcount = m_messages.count();
    m_messages.clear();

    QList<QString> results;
    QRegExp subjectMatch("Subject: +(.*)");
    QRegExp fromMatch("From: +(.*)");
    QRegExp dateMatch("Date: +(.*)");

    for(int mbox = 0; mbox < mailBoxes.length(); mbox++) {

        DEBUG("---- " << mailBoxes[mbox].toAscii().data() << " ------\n");

        sendCommand(QString("EXAMINE \"" + mailBoxes[mbox] + "\""));

        results = sendCommand(QString("SEARCH RECENT"));
        QStringList msglist = results[0].split(' ');
        for(int i = 2; i < msglist.length(); i++) {
            QString subject, from, date;
            QStringList headers =
                sendCommand(QString("FETCH " + msglist[i] +
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

            m_messages.push_back(MailMsg(mailBoxes[mbox], date, from, subject));

            date.truncate(DATE_WIDTH);
            from.truncate(FROM_WIDTH);
            subject.truncate(SUBJECT_WIDTH);
            QString output = QString("%1 | %2 | %3")
                .arg(date, - DATE_WIDTH)
                .arg(from, - FROM_WIDTH)
                .arg(subject, - SUBJECT_WIDTH);
            DEBUG(output.toAscii().data() << "\n");
        }
    }
    emit mailUpdated();
    emit updateCount(oldcount, m_messages.count());
    if (oldcount < m_messages.count())
        emit newMail();
    m_statusMessage = QString("%1 messages available").arg(m_messages.count());
    emit statusMessage(m_statusMessage, 0);
}

QList<QString>
IncomingMailModel::sendCommand(const QString &cmd) {
    char buf[1024];

    QString fullcmd ( QString('A') + QString::number(++__counter) +
                      QString(' ') + cmd + QString('\n') );
    DEBUG ("sending: " << fullcmd.toAscii().data());
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
        DEBUG( "data: " << resultString.toAscii().data() << "\n");

        if (donematch.indexIn(resultString) != -1) {
            notDone = false;
            DEBUG( "end marker found " << donematch.pattern().toAscii().data() << "\n");
        }
    }
    DEBUG( "----\n");
    return results;
}

void
IncomingMailModel::setupTimer()
{
    m_timer.stop();
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(checkMail()));
    m_timer.start(m_checkinterval);
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


#include "moc_incomingmailmodel.cpp"

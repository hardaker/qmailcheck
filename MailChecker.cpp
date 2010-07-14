#include "MailChecker.h"
#include "qmailcheckcommon.h"

#define DATE_WIDTH    5
#define FROM_WIDTH    20
#define SUBJECT_WIDTH 48

MailChecker::MailChecker(IncomingMailModel *model, QMutex *mutex, QObject *parent) :
    QThread(parent), m_model(model), m_mutex(mutex)
{
}

void MailChecker::run() {

}

void MailChecker::shutDown() {

}

void MailChecker::initializeSocket()
{
    if (!m_model->m_socket.isOpen() && m_model->m_mailSources.count() > 0) {
        QList<MailSource *>::iterator     source;
        QList<MailSource *>::iterator     sourceEnd = m_model->m_mailSources.end();

        for(source = m_model->m_mailSources.begin(); source != sourceEnd; ++source) {

            DEBUG("Opening Connection\n");
            qDebug() << (*source)->hostName() << "/" << (*source)->portNumber();

            if (true || (*source)->ignoreCertErrors())
                m_model->m_socket.setPeerVerifyMode(QSslSocket::VerifyNone);
            m_model->m_socket.connectToHostEncrypted((*source)->hostName(), (*source)->portNumber());
            m_model->m_socket.waitForReadyRead();

            // read throw-away line info
            char buf[4096];
            m_model->m_socket.readLine( buf, sizeof( buf ) );

            DEBUG("intial line: " << buf);

            // XXX: should check for error
            sendCommand(QString("login ") + (*source)->userName() + " " + (*source)->passPhrase());
            // XXX: A1 NO [AUTHENTICATIONFAILED] Authentication failed.

            setupTimer();
        }
    }
}

void MailChecker::reInitializeSocket()
{
    if (m_model->m_socket.isOpen())
        m_model->m_socket.close();
    initializeSocket();
}

QList<QString> MailChecker::sendCommand(const QString &cmd) {
    char buf[1024];

    QString fullcmd ( QString('A') + QString::number(++__counter) +
                      QString(' ') + cmd + QString('\n') );
    //DEBUG ("sending: " << fullcmd.toAscii().data());
    m_model->m_socket.write(fullcmd.toAscii().data(), fullcmd.length());

    QRegExp donematch(QString("^A") + QString::number(__counter) + QString(" "));

    bool notDone = true;
    QList<QString> results;
    while(notDone) {
        if ( m_model->m_socket.bytesAvailable() <= 0 )
            m_model->m_socket.waitForReadyRead();

        int linelength = m_model->m_socket.readLine( buf, sizeof( buf ) );
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
MailChecker::setupTimer()
{
    m_timer.stop();
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(checkMail()));
    m_timer.start(m_model->m_checkinterval * 1000);
}

void MailChecker::checkMail()
{

    DEBUG("Attempting to check mail...\n");

    if (!m_model->folderList || m_model->folderList->count() == 0) {
        if (!m_model->folderList) {
            DEBUG("no folder list\n");
        } else {
            DEBUG("folder list empty\n");
        }
        return;
    }

    if (! m_model->m_socket.isOpen()) {
        DEBUG("socket not open\n");
        initializeSocket();
        if (! m_model->m_socket.isOpen())
            return;
    }

    QMap<QString, bool> uid_list;
    for(int i = 0; i < m_model->m_messages.count(); i++) {
        uid_list.insert(m_model->m_messages[i].uid(), m_model->m_messages[i].isnew());
    }

    int oldcount = m_model->m_messages.count();
    m_model->m_messages.clear();

    QList<QString> results;
    QRegExp subjectMatch("Subject: +(.*)");
    QRegExp fromMatch("From: +(.*)");
    QRegExp dateMatch("Date: +(.*)");
    bool containsNewMessages = false;

    for(int mbox = 0; mbox < m_model->folderList->count(); mbox++) {

        if (m_model->folderList->folderName(mbox) == "")
            continue;

        //DEBUG("---- " << m_model->folderList->folderName(mbox).toAscii().data() << " ------\n");

        sendCommand(QString("EXAMINE \"" + m_model->folderList->folderName(mbox) + "\""));

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

            MailMsg message(msglist[i], m_model->folderList->folderName(mbox), date, from, subject);
            if (m_model->m_highlightNew && ! uid_list.contains(msglist[i])) {
                message.setIsNew(true);
                containsNewMessages = true;
                emit newMailMessage(from + "\n" + subject);
            } else {
                message.setIsNew(uid_list[msglist[i]]);
            }

            m_model->m_messages.push_back(message);

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
    emit updateCount(oldcount, m_model->m_messages.count());
    if (containsNewMessages) {
        emit newMail();
        emitChanges();
    }
    m_model->m_statusMessage = QString("%1 messages available").arg(m_model->m_messages.count());
    emit statusMessage(m_model->m_statusMessage, 0);
}

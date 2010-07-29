#include "MailChecker.h"
#include "qmailcheckcommon.h"
#include "qtincoming.h"

#define DATE_WIDTH    5
#define FROM_WIDTH    20
#define SUBJECT_WIDTH 48

MailChecker::MailChecker(IncomingMailModel *model, QMutex *mutex, MailSource *mailSource, folderModel *folderModel, int checkInterval,
                         QList<MailMsg> *messages) :
    QThread(0), m_socket(0), m_timer(0), m_model(model), m_mutex(mutex), m_mailSource(mailSource), m_folderModel(folderModel),
    m_messages(messages), m_checkingNow(false)
{
    qDebug() << "----- New CHECKER";
}

void MailChecker::connectSignals(QTableView *mailView, QtIncoming *mainWidget)
{
    // signals from the mailbox
    connect(this, SIGNAL(mailUpdated()),
            mailView, SLOT(resizeRowsToContents()));
    connect(this, SIGNAL(mailUpdated()),
            mailView, SLOT(resizeColumnsToContents()));
    connect(this, SIGNAL(mailUpdated()),
            mailView, SLOT(repaint()));

    connect(this, SIGNAL(updateCount(int, int)),
            mailView, SLOT(rowCountChanged(int, int)));

    connect(this, SIGNAL(newMail()),
            mainWidget, SLOT(maybeRaise()));
    connect(this, SIGNAL(mailUpdated()),
            mailView, SLOT(repaint()));

    // don't do popups for the first mail check
    connect(this, SIGNAL(newMail()),
            mainWidget, SLOT(newMail()));

    // connect this after the initial check mail pass
    connect(this, SIGNAL(newMailMessage(QString)),
            mainWidget, SLOT(sendNotification(QString)));

    connect(this, SIGNAL(internalCheckMailSignal()), this, SLOT(checkMail()));
}

void MailChecker::internalCheckMail() {
    if (!m_checkingNow)
        QTimer::singleShot(0, this, SLOT(checkMail()));
}

void MailChecker::shutDown() {
    qDebug() << "----- SHUTDOWN CALLED";
    if (m_socket)
        m_socket->close();
}

void MailChecker::initializeSocket()
{
    qDebug() << "----- INIT SOCKET CALLED";
    if (m_socket)
        delete m_socket;
    m_socket = new QSslSocket(this);

    QList<QString> results;

    qDebug() << "Opening Connection to: " << m_mailSource->hostName() << "/" << m_mailSource->portNumber();

    if (true || m_mailSource->ignoreCertErrors())
        m_socket->setPeerVerifyMode(QSslSocket::VerifyNone);
    m_socket->connectToHostEncrypted(m_mailSource->hostName(), m_mailSource->portNumber());
    if (!m_socket->isOpen()) {
        qWarning() << "failed to connect to " << m_mailSource->hostName() << ":" << m_mailSource->portNumber();
        sleep(1);
        return;
    }
    m_socket->waitForReadyRead();

    // read throw-away line info
    char buf[4096];
    m_socket->readLine( buf, sizeof( buf ) );

    DEBUG("intial line: " << buf);

    // XXX: should check for error
    results = sendCommand(QString("login ") + m_mailSource->userName() + " " + m_mailSource->passPhrase());
    qDebug() << "login results:" << results;
    if (results.length() == 0) {
        qWarning() << "login failed";
        m_socket->close();
        return;
    }
    // XXX: A1 NO [AUTHENTICATIONFAILED] Authentication failed.
}

void MailChecker::reInitializeSocket()
{
    if (m_socket->isOpen()) {
        m_socket->close();
        sleep(1);
    }
    initializeSocket();
}

QList<QString> MailChecker::sendCommand(const QString &cmd) {
    char buf[1024];

    QString fullcmd ( QString('A') + QString::number(++__counter) +
                      QString(' ') + cmd + QString('\n') );
    DEBUG ("sending: " << fullcmd.toAscii().data());
    m_socket->write(fullcmd.toAscii().data(), fullcmd.length());

    QRegExp donematch(QString("^A") + QString::number(__counter) + QString(" "));

    bool notDone = true;
    QList<QString> results;
    while(notDone) {
        if ( m_socket->bytesAvailable() <= 0 )
            m_socket->waitForReadyRead();

        int linelength = m_socket->readLine( buf, sizeof( buf ) );
        if (linelength <= 0) {
            return QList<QString>(); // hopefully they check for an empty list
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

void MailChecker::checkMail()
{

    DEBUG("Attempting to check mail...\n");

    if (!m_folderModel || m_folderModel->count() == 0) {
        if (!m_folderModel) {
            DEBUG("no folder list\n");
        } else {
            DEBUG("folder list empty\n");
        }
        return;
    }

    if (!m_socket || ! m_socket->isOpen()) {
        DEBUG("socket not open\n");
        initializeSocket();
        if (!m_socket || ! m_socket->isOpen())
            return;
    }

    QMap<QString, bool> uid_list;
    for(int i = 0; i < m_messages->count(); i++) {
        uid_list.insert((*m_messages)[i].uid(), (*m_messages)[i].isnew());
    }

    int oldcount = m_messages->count();
    m_messages->clear();

    QList<QString> results;
    QRegExp subjectMatch("Subject: +(.*)");
    QRegExp fromMatch("From: +(.*)");
    QRegExp dateMatch("Date: +(.*)");
    bool containsNewMessages = false;

    m_checkingNow = true;

    for(int mbox = 0; mbox < m_folderModel->count(); mbox++) {

        if (m_folderModel->folderName(mbox) == "")
            continue;

        DEBUG("---- " << m_folderModel->folderName(mbox).toAscii().data() << " ------\n");

        sendCommand(QString("EXAMINE \"" + m_folderModel->folderName(mbox) + "\""));

        results = sendCommand(QString("UID SEARCH RECENT"));
        if (results.length() == 0) {
            // socket probably died.
            reInitializeSocket();
            m_checkingNow = false;
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

            MailMsg message(msglist[i], m_folderModel->folderName(mbox), date, from, subject);
            if (! uid_list.contains(msglist[i])) {
                message.setIsNew(true);
                containsNewMessages = true;
                emit newMailMessage(from + "\n" + subject);
            } else {
                message.setIsNew(uid_list[msglist[i]]);
            }

            m_messages->push_back(message);

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
    qDebug() << "Done checking mail";
    emit mailUpdated();
    emit updateCount(oldcount, m_messages->count());
    if (containsNewMessages) {
        emit newMail();
        // m_model->emitChanges();
    }
    m_statusMessage = QString("%1 messages available").arg(m_messages->count());
    emit statusMessage(m_statusMessage, 0);
    m_checkingNow = false;
}

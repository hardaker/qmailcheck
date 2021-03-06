#include "MailChecker.h"
#include "qmailcheckcommon.h"
#include "folderitem.h"
#include "qtincoming.h"
#include <QDateTime>
#include <QDate>

#define DATE_WIDTH    5
#define FROM_WIDTH    20
#define SUBJECT_WIDTH 48

MailChecker::MailChecker(IncomingMailModel *model, QMutex *mutex, MailSource *mailSource, folderModel *folderModel, int checkInterval,
                         QList<MailMsg> *messages) :
    QThread(0), m_socket(0), m_timer(0), m_model(model), m_mutex(mutex), m_mailSource(mailSource), m_folderModel(folderModel),
    m_messages(messages), m_checkingNow(false), m_cachedMessages(), uid_list()
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

    connect(this, SIGNAL(updateCount(int, int)),
            mailView, SLOT(rowCountChanged(int, int)));

    //connect(this, SIGNAL(newMail()),
    //        mainWidget, SLOT(maybeRaise()));  // now handled below
    connect(this, SIGNAL(mailUpdated()),
            mailView, SLOT(repaint()));

    // don't do popups for the first mail check
    connect(this, SIGNAL(newMail()),
            mainWidget, SLOT(newMail()));

    // connect this after the initial check mail pass
    connect(this, SIGNAL(newMailMessage(QString, bool)),
            mainWidget, SLOT(sendNotification(QString, bool)));

    // notification handling
    connect(this, SIGNAL(LEDNotification()),
            mainWidget, SLOT(doLED()));
    connect(this, SIGNAL(notificationNotification()),
            mainWidget, SLOT(doNotification()));
    connect(this, SIGNAL(vibrateNotification()),
            mainWidget, SLOT(doVirbrate()));
    connect(this, SIGNAL(soundNotification()),
            mainWidget, SLOT(doSound()));
    connect(this, SIGNAL(popupNotification()),
            mainWidget, SLOT(doPopup()));

    connect(this, SIGNAL(internalCheckMailSignal()), this, SLOT(checkMail()));
}

void MailChecker::run() {
    qDebug() << "----- RUN CALLED";
    //setupTimer();
    //checkMail();
    exec();
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

    if (m_mailSource->ignoreCertErrors())
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
    if (m_socket->readLine( buf, sizeof( buf ) ) < 4) {
        // arbitrary length expectation, but it should certainly be > 4
        qWarning() << "failed to read the initial imap connection line";
        m_socket->close();
        return;
    }

    DEBUG("intial line: " << buf);

    // XXX: should check for error
    QString userName = m_mailSource->userName();
    QString passPhrase = m_mailSource->openPassPhrase(userName + "@" + m_mailSource->hostName());

    if (passPhrase.isEmpty() || userName.isEmpty()) {
        qWarning() << "login info missing";
        m_socket->close();
        return;
    }

    results = sendCommand(QString("login ") + userName  + " " + passPhrase);
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

QList<QString> MailChecker::sendCommand(const QString &cmd, bool debugOutput) {
    char buf[1024];

    QString fullcmd ( QString('A') + QString::number(++__counter) +
                      QString(' ') + cmd + QString('\n') );
    if (debugOutput)
        qDebug() << "sending: " << fullcmd;
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
        if (debugOutput) {
            qDebug() << "data: " << resultString.toAscii().data();
        }

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

    QString searchString = "UID SEARCH RECENT";
    if (m_model->useUnseen()) {
        QDate myDate = QDate::currentDate();
        QString searchDate = myDate.toString("d-MMM-yyyy");
        searchString = "UID SEARCH UNSEEN SINCE " + searchDate;
    }

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
        if (!m_socket || ! m_socket->isOpen()) {
            emit mailUpdated();
            return;
        }
    }

    uid_list.clear();
    for(int i = 0; i < m_messages->count(); i++) {
        uid_list.insert((*m_messages)[i].uid(), (*m_messages)[i].isnew());
    }

    int oldcount = m_messages->count();
    m_messages->clear();

    QList<QString> results;
    QRegExp subjectMatch("Subject: +(.*)");
    QRegExp fromMatch("From: +(.*)");
    QRegExp dateMatch("Date: +(.*)");
    QRegExp newMessageMatch("^\\*.*FETCH.*UID ([0-9]+)");
    QRegExp flagsMatch("FLAGS +\\(.*\\)");

    m_checkingNow = true;
    doLED = false; doVibrate = false; doNotification = false; doPopup = false; doSound = false;
    containsNewMessages = false;

    // RFC3501: imap4

    for(int mbox = 0; mbox < m_folderModel->count(); mbox++) {
        folderItem &folder = m_folderModel->folder(mbox);

        if (folder.folderName() == "")
            continue;

        DEBUG("---- " << folder.folderName().toAscii().data() << " ------\n");

        sendCommand(QString("EXAMINE \"" + folder.folderName() + "\""));

        results = sendCommand(searchString);
        if (results.length() == 0) {
            // socket probably died.
            reInitializeSocket();
            m_checkingNow = false;
            emit mailUpdated();
            return;
        }

        QStringList msglist = results[0].split(' ');
        QStringList fetchList;
        if (!msglist.isEmpty()) {
            msglist.pop_front(); // get rid of the prefixes
        }
        if (!msglist.isEmpty()) {
            msglist.pop_front();
        }

        foreach(stringpair index, m_cachedMessages.keys()) {
            if (index.first == folder.folderName() &&
                    !msglist.contains(index.second)) {
                // the message has disappeared; nuke it from our cache
                m_cachedMessages.remove(index);
            }
        }

        // checked for cached copies
        // mark messages we've never seen for fetching data for
        for(int i = 0; i < msglist.length(); i++) {
            if (m_cachedMessages.contains(QPair<QString, QString>(folder.folderName(), msglist[i]))) {
                foundMessage(folder, folder.folderName(),
                             m_cachedMessages[QPair<QString, QString>(folder.folderName(), msglist[i])]);
            } else {
                fetchList.push_back(msglist[i]);
            }
        }

        // pull anything not cached
        QString subject, from, date, uid;

        if (fetchList.count() > 0) {
            QStringList headers =
                    sendCommand(QString("UID FETCH " + fetchList.join(",") +
                                        " (FLAGS BODY[HEADER.FIELDS (SUBJECT FROM DATE)])"));
            qDebug() << "headers: " << headers;
            for(int j = 0; j < headers.length(); j++) {
                if (newMessageMatch.indexIn(headers[j]) != -1) {
                    if (uid.length() > 0) {
                        foundMessage(folder, folder.folderName(),
                                     MailMsg(uid, folder.displayName(), date, from, subject));
                    }
                    uid = newMessageMatch.cap(1);
                } else if (subjectMatch.indexIn(headers[j]) != -1) {
                    subject = subjectMatch.cap( 1 );
                } else if (fromMatch.indexIn(headers[j]) != -1) {
                    from = fromMatch.cap( 1 );
                } else if (dateMatch.indexIn(headers[j]) != -1) {
                    date = dateMatch.cap( 1 );
                } else if (flagsMatch.indexIn(headers[j]) != -1) {
                    qDebug() << "found flags: " << headers[j];
                }
            }

            if (uid.length() > 0) {
                foundMessage(folder, folder.folderName(),
                             MailMsg(uid, folder.displayName(), date, from, subject));
            }
        }
    }

    qDebug() << "Done checking mail";
    emit updateCount(oldcount, m_messages->count());
    emit mailUpdated();

    if (doLED)
        emit LEDNotification();
    if (doNotification)
        emit notificationNotification();
    if (doPopup)
        emit popupNotification();
    if (doVibrate)
        emit vibrateNotification();
    if (doSound)
        emit soundNotification();

    if (containsNewMessages) {
        emit newMail();
        // m_model->emitChanges();
    }
    m_statusMessage = QString("%1 messages available").arg(m_messages->count());
    emit statusMessage(m_statusMessage, 0);
    m_checkingNow = false;
}

void MailChecker::foundMessage(const folderItem &folder, const QString &folderName, MailMsg message) {
    //qDebug() << " --- found message " << message.uid();
    m_cachedMessages.insert(QPair<QString, QString>(folderName, message.uid()), message);

    if (! uid_list.contains(message.uid())) {
        message.setIsNew(true);
        containsNewMessages = true;
        emit newMailMessage(message.from() + "\n" + message.subject(), folder.doNotification());

        // figure out which notifications might need emiting
        if (folder.doLED())
            doLED = true;
        if (folder.doNotification())
            doNotification = true;
        if (folder.doPopup())
            doPopup = true;
        if (folder.doVibrate())
            doVibrate = true;
        if (folder.doSound())
            doSound = true;

    } else {
        message.setIsNew(uid_list[message.uid()]);
    }

    m_messages->push_back(message);
}

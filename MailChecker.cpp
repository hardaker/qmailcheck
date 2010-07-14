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

void
MailChecker::checkMail()
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

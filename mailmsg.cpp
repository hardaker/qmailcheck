#include "mailmsg.h"

#include <QRegExp>

MailMsg::MailMsg() 
    : m_folder(), m_date(), m_from(), m_subject(), m_uid(), m_isnew(false)
{
    setTime();
}

MailMsg::MailMsg(const QString &uid, const QString &folder, const QString &date,
                 const QString &from, const QString &subject) 
    : m_folder(folder), m_date(date), m_from(from),
      m_subject(subject), m_uid(uid), m_isnew(false)
{
    setTime();
}

MailMsg::~MailMsg()
{
}

void
MailMsg::setSubject(QString &subject)
{
    m_subject = subject;
}

void
MailMsg::setFrom(QString &from)
{
    m_from = from;
}

void
MailMsg::setDate(QString &date)
{
    m_date = date;
    setTime();
}

void
MailMsg::setTime()
{
    QRegExp timematch("([0-9][0-9]:[0-9][0-9])");
    if (timematch.indexIn(m_date) != -1) {
        m_time = timematch.cap(1);
    } else {
        m_time = "?";
    }
}

void
MailMsg::setFolder(QString &folder)
{
    m_folder = folder;
}

void
MailMsg::setUid(QString &uid)
{
    m_uid = uid;
}

void
MailMsg::setIsNew(bool isnew)
{
    m_isnew = isnew;
}

const QString &
MailMsg::subject() const
{
    return m_subject;
}

const QString &
MailMsg::date() const
{
    return m_date;
}

const QString &
MailMsg::from() const
{
    return m_from;
}

const QString &
MailMsg::folder() const
{
    return m_folder;
}

const QString &
MailMsg::uid() const
{
    return m_uid;
}

bool
MailMsg::isnew() const
{
    return m_isnew;
}

const QString &
MailMsg::time() const
{
    return m_time;
}


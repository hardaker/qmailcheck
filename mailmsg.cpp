#include "mailmsg.h"

#include <QRegExp>

MailMsg::MailMsg() 
    : m_date(), m_from(), m_subject()
{
    setTime();
}

MailMsg::MailMsg(QString &date, QString &from, QString &subject) 
    : m_date(date), m_from(from), m_subject(subject)
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
MailMsg::time() const
{
    return m_time;
}


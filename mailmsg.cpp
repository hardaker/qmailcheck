#include "mailmsg.h"

MailMsg::MailMsg() 
    : m_date(), m_from(), m_subject()
{
}

MailMsg::MailMsg(QString &date, QString &from, QString &subject) 
    : m_date(date), m_from(from), m_subject(subject)
{
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


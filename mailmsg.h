#ifndef MAILMSG_H
#define MAILMSG_H

#include <QtCore/QString>

class MailMsg {
  public:

    MailMsg();
    MailMsg(QString &date, QString &from, QString &subject);

    ~MailMsg();

    const QString &subject() const;
    const QString &from() const;
    const QString &date() const;

    void setSubject(QString &subject);
    void setDate(QString &date);
    void setFrom(QString &from);

  private:
    QString m_date;
    QString m_from;
    QString m_subject;
};
#endif /* MAILMSG_H */

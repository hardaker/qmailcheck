#ifndef MAILMSG_H
#define MAILMSG_H

#include <QtCore/QString>

class MailMsg {
  public:

    MailMsg();
    MailMsg(QString &folder, QString &date, QString &from, QString &subject);

    ~MailMsg();

    const QString &subject() const;
    const QString &from() const;
    const QString &date() const;
    const QString &time() const;
    const QString &folder() const;

    void setSubject(QString &subject);
    void setDate(QString &date);
    void setFrom(QString &from);
    void setFolder(QString &folder);

  protected:
    void setTime();

  private:
    QString m_folder;
    QString m_date;
    QString m_time;
    QString m_from;
    QString m_subject;
};
#endif /* MAILMSG_H */

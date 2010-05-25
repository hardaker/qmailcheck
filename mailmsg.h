#ifndef MAILMSG_H
#define MAILMSG_H

#include <QtCore/QString>

class MailMsg {
  public:

    MailMsg();
    MailMsg(const QString &m_uid, const QString &folder, const QString &date, const QString &from, const QString &subject);

    ~MailMsg();

    const QString &subject() const;
    const QString &from() const;
    const QString &date() const;
    const QString &time() const;
    const QString &folder() const;
    const QString &uid() const;
    bool     isnew() const;

    void setSubject(QString &subject);
    void setDate(QString &date);
    void setFrom(QString &from);
    void setFolder(QString &folder);
    void setUid(QString &uid);
    void setIsNew(bool isnew);

  protected:
    void setTime();

  private:
    QString m_folder;
    QString m_date;
    QString m_time;
    QString m_from;
    QString m_subject;
    QString m_uid;
    bool m_isnew;
};
#endif /* MAILMSG_H */

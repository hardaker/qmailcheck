#ifndef MAILCHECKER_H
#define MAILCHECKER_H

#include <QThread>

class MailChecker : public QThread
{
    Q_OBJECT
public:
    explicit MailChecker(QObject *parent = 0);

signals:

public slots:

};

#endif // MAILCHECKER_H

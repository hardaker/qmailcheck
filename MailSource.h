#ifndef MAILSOURCE_H
#define MAILSOURCE_H

#include <QObject>

class MailSource : public QObject
{
    Q_OBJECT
public:
    explicit MailSource(QObject *parent = 0);

signals:

public slots:

};

#endif // MAILSOURCE_H

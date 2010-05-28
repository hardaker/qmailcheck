#ifndef FOLDERITEM_H
#define FOLDERITEM_H

#include <QString>

class folderItem
{
public:
    folderItem();
    folderItem(const QString &folderName, const QString &displayName,
               bool doNotification);

    const QString &folderName() const;
    const QString &displayName() const;
    bool           doNotification() const;

    void set_folderName(QString &folderName);
    void set_displayName(QString &displayName);
    void set_doNotification(bool doNotification);

  private:
    QString m_folderName;
    QString m_displayName;
    bool    m_doNotification;

};

#endif // FOLDERITEM_H

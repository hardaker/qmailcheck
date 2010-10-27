#ifndef FOLDERITEM_H
#define FOLDERITEM_H

#include <QString>

class folderItem
{
public:

    folderItem(const QString &folderName = "", const QString &displayName = "",
               bool doNotification = false, bool doPopup = false, bool doVibrate = false,
               bool doLED = false);

    const QString &folderName() const;
    const QString &displayName() const;
    bool           doNotification() const;

    void set_folderName(QString &folderName);
    void set_displayName(QString &displayName);
    void set_doNotification(bool doNotification);

    bool           doPopup() const;
    void           set_doPopup(bool doPopup);

    bool           doVibrate() const;
    void           set_doVibrate(bool doVibrate);

    bool           doLED() const;
    void           set_doLED(bool doLED);

  private:
    QString m_folderName;
    QString m_displayName;
    bool    m_doNotification;
    bool    m_doPopup;
    bool    m_doVibrate;
    bool    m_doLED;
};

#endif // FOLDERITEM_H

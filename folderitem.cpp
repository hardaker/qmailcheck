#include "folderitem.h"

folderItem::folderItem(const QString &folderName, const QString &displayName,
                       bool doNotification, bool doPopup, bool doVibrate,
                       bool doLED)
    : m_folderName(folderName), m_displayName(displayName),
      m_doNotification(doNotification), m_doPopup(doPopup), m_doVibrate(doVibrate), m_doLED(doLED)
{
}

const QString &
folderItem::folderName() const 
{
    return m_folderName;
}

const QString &
folderItem::displayName() const 
{
    return m_displayName;
}

bool
folderItem::doNotification() const
{
    return m_doNotification;
}

void
folderItem::set_folderName(QString &folderName)
{
    m_folderName = folderName;
}

void
folderItem::set_displayName(QString &displayName)
{
    m_displayName = displayName;
}

void
folderItem::set_doNotification(bool doNotification)
{
    m_doNotification = doNotification;
}

bool folderItem::doPopup() const
{
    return m_doPopup;
}

void folderItem::set_doPopup(bool doPopup)
{
    m_doPopup = doPopup;
}


bool folderItem::doVibrate() const
{
    return m_doVibrate;
}

void folderItem::set_doVibrate(bool doVibrate)
{
    m_doVibrate = doVibrate;
}


bool folderItem::doLED() const
{
    return m_doLED;
}

void folderItem::set_doLED(bool doLED)
{
    m_doLED = doLED;
}

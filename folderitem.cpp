#include "folderitem.h"

folderItem::folderItem()
    : m_folderName(), m_displayName(), m_doNotification(true)
{
}

folderItem::folderItem(const QString &folderName, const QString &displayName,
                       bool doNotification)
    : m_folderName(folderName), m_displayName(displayName),
      m_doNotification(doNotification)
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


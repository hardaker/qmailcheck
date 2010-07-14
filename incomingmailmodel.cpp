#include "incomingmailmodel.h"
#include "MailChecker.h"

#include <QtGui/QLabel>
#include <QtNetwork/QSslSocket>

#define DATE_WIDTH    5
#define FROM_WIDTH    20
#define SUBJECT_WIDTH 48

#include "qmailcheckcommon.h"

enum column_list 
{
    COL_FOLDER  = 0,
    COL_DATE    = 1,
    COL_FROM    = 2,
    COL_SUBJECT = 3
};

IncomingMailModel::IncomingMailModel(QObject *parent) :
    QAbstractTableModel(parent), m_socket(), m_checker(0), m_mutex(new QMutex()),
    folderList(0), m_messages(), m_hideList(), m_checkinterval(600), m_statusMessage(), m_highlightNew(true)
{
}

void IncomingMailModel::changedSettings() {
    restartCheckers();
}

void IncomingMailModel::restartCheckers() {
    if (m_checker) {
        m_checker->shutDown();
        delete m_checker;
    }
    m_checker = new MailChecker(this, m_mutex, 0);
    m_checker->start();
}

int IncomingMailModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_messages.length();
    //return 20;
}

int IncomingMailModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

Qt::ItemFlags IncomingMailModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    
    return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled;
}

QVariant IncomingMailModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QList<MailMsg>::const_iterator message;
    QList<MailMsg>::const_iterator stopat = m_messages.end();
    int findrow = index.row();
    QString lastfolder;
    
    for(message = m_messages.begin(); message != stopat; ++message) {
        if (! m_hideList.contains(message->uid()))
            findrow--;
        if (findrow == -1)
            break;
        if (message == stopat)
            return QVariant();
        lastfolder = message->folder();
    }

    if (message == stopat)
        return QVariant();

    if (index.column() > COL_SUBJECT)
        return QVariant();

    if (role == Qt::FontRole) {
        return m_font;
    }

    if (false && Qt::SizeHintRole == role){
        QSize s;
        s.setHeight(12);
        s.setWidth(400);
        return s;
        return (QVariant(m_font.pointSize()));
        QVariant size = QAbstractTableModel::data(index, Qt::SizeHintRole);
        if (size.toInt() > 5)
            return QVariant(size.toInt() - 5);
        else
            return size;
    }

    if (role == Qt::BackgroundRole) {
        // QColor(128,255,255);n
        // return QApplication::palette().colorGroup(QPalette::Highlight);
        if (message->isnew())
#if defined(Q_WS_MAEMO_5) || defined(MAEMO_CHANGES)
            return QColor(255,255,255);
#else
            return QApplication::palette().highlight();
#endif
        else
            return QVariant();
    }

    if (role == Qt::ForegroundRole) {
        if (message->isnew())
#if defined(Q_WS_MAEMO_5) || defined(MAEMO_CHANGES)
            return QColor(128,128,255);
#else
            return QApplication::palette().highlightedText();
#endif
        else
            return QVariant();
    }

    if (role != Qt::DisplayRole)
        return QVariant();

    if (index.column() > 4)
        return QVariant();

    switch(index.column()) {

    case COL_FOLDER:
        if (message->folder() == lastfolder)
            return QVariant();
        return message->folder();

    case COL_DATE:
        return message->time();

    case COL_FROM:
        return message->from();

    case COL_SUBJECT:
        return message->subject();

    default:
        return QVariant();
        
    }
}

QVariant IncomingMailModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section);
    Q_UNUSED(orientation);
    Q_UNUSED(role);
    
#if 0
    if (role == Qt::SizeHintRole)
        return QSize(1,1);
#endif
    return QVariant();
#if 0
    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg("section");
    else
        return QString("Row %1").arg("section");
#endif
}

void IncomingMailModel::readSettings(QSettings &settings, Ui::PrefWindow *prefui) {
    if (m_mailSources.count() == 0) {
        m_mailSources.append(new MailSource());
    }

    QList<MailSource *>::iterator source = m_mailSources.begin();

    prefui->serverName->setText(settings.value("serverName").toString());
    (*source)->set_hostName(QString(settings.value("serverName", "localhost").toString()));

    prefui->password->setText(settings.value("password").toString());
    (*source)->set_passPhrase(settings.value("password", "").toString());

    prefui->userName->setText(settings.value("username").toString());
    (*source)->set_userName(settings.value("username", "").toString());

    prefui->serverPort->setText(settings.value("serverPort").toString());
    (*source)->set_portNumber(settings.value("serverPort", 993).toInt());

    set_font(settings.value("font").value<QFont>());
}

void IncomingMailModel::saveSettings(QSettings &settings, Ui::PrefWindow *prefui) {
    settings.setValue("serverName", prefui->serverName->text());
    settings.setValue("serverPort", prefui->serverPort->text());
    settings.setValue("userName", prefui->userName->text());
    settings.setValue("password", prefui->password->text());
    settings.setValue("font", m_font);
}

void IncomingMailModel::set_checkinterval(int checkinterval) {
    m_checkinterval = checkinterval;
}

void IncomingMailModel::set_folderList(folderModel *list) 
{
    folderList = list;
}

void IncomingMailModel::set_highlightNew(bool newval)
{
    m_highlightNew = newval;
}

void IncomingMailModel::set_font(const QFont &font) {
    m_font = font;
}

QFont IncomingMailModel::font() {
    return m_font;
}

void IncomingMailModel::clearHideList()
{
    m_hideList.clear();
    emitChanges();
}

void IncomingMailModel::hideMessages()
{
    QList<MailMsg>::iterator stopAt = m_messages.end();
    QList<MailMsg>::iterator message;
    for(message = m_messages.begin(); message != stopAt; ++message) {
        if (! m_hideList.contains(message->uid()))
            m_hideList.push_back(message->uid());
    }
    emitChanges();
}

void IncomingMailModel::clearNew()
{
    for(int i = 0; i < m_messages.count(); i++) {
        m_messages[i].setIsNew(false);
    }
    emitChanges();
}

void IncomingMailModel::emitChanges()
{
    emit dataChanged(createIndex(0,0,0),
                     createIndex(m_messages.count(),3,0));
    emit layoutChanged();
}

#include "moc_incomingmailmodel.cpp"

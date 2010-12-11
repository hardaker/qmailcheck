
#include "incomingmailmodel.h"
#include "MailChecker.h"

#include <QtGui/QLabel>
#include <QtNetwork/QSslSocket>
#include "qtincoming.h"

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

IncomingMailModel::IncomingMailModel(QObject *parent, QtIncoming *mainWidget, QTableView *mailView) :
    QAbstractTableModel(parent), m_timer(parent), m_mainWidget(mainWidget), m_mailView(mailView), m_checker(0), m_mutex(new QMutex()),
    folderList(0), m_messages(), m_hideList(), m_checkinterval(600), m_highlightNew(true), m_statusMessage()
{
    //setupTimer();
    QTimer::singleShot(1000, this, SLOT(checkMailSlot()));
}

void IncomingMailModel::changedSettings() {
    restartCheckers();
}

void IncomingMailModel::checkMailSlot() {
    emit checkMail();
}

void IncomingMailModel::restartTimer() {
    qDebug() << "restarting the timer for " << m_checkinterval << "s";
    QTimer::singleShot(m_checkinterval * 1000, this, SLOT(checkMailSlot()));
}

void IncomingMailModel::restartCheckers() {
    if (m_checker) {
        m_checker->shutDown();
        m_checker->wait(10);
        delete m_checker;
    }
    m_checker = new MailChecker(this, m_mutex, m_mailSources[0], folderList, m_checkinterval, &m_messages);
    m_checker->connectSignals(m_mailView, m_mainWidget);
    m_checker->start();
    connect(this, SIGNAL(checkMail()), m_checker, SLOT(internalCheckMail()));
    connect(m_checker, SIGNAL(mailUpdated()), this, SLOT(restartTimer()));
    connect(m_checker, SIGNAL(mailUpdated()), this, SLOT(emitChanges()));
    qDebug() << "past start of checker";
    //emit checkMail(); // immediate check
}

void
IncomingMailModel::setupTimer()
{
    qDebug() << "setting up timer";
    m_timer.stop();
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(checkMailSlot()));
    m_timer.start(m_checkinterval * 1000);
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
    
    Qt::ItemFlags flags = QAbstractItemModel::flags(index) | Qt::ItemIsEnabled;
    if (flags & Qt::ItemIsSelectable)
      flags &= ~Qt::ItemIsSelectable;

    return flags;
}

QVariant IncomingMailModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QList<MailMsg>::const_iterator message;
    QList<MailMsg>::const_iterator stopat = m_messages.end();
    int findrow = index.row();
    QString lastfolder;
    
    if (index.column() > COL_SUBJECT)
        return QVariant();

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

    if (role == Qt::FontRole) {
        return m_font;
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
        return QColor(0,0,0);
        if (message->isnew())
#if defined(Q_WS_MAEMO_5) || defined(MAEMO_CHANGES)
            return QColor(128,128,255);
#else
            return QApplication::palette().highlightedText();
#endif
        else
            return QVariant();
    }


    if (role != Qt::DisplayRole && role != Qt::SizeHintRole)
        return QVariant();

    if (index.column() > 4)
        return QVariant();

    QString value;
    switch(index.column()) {

    case COL_FOLDER:
        if (message->folder() == lastfolder)
            return QVariant();
        value = message->folder();
        break;

    case COL_DATE:
        value = message->time();
        break;

    case COL_FROM:
        value = message->from();
        break;

    case COL_SUBJECT:
        value = message->subject();
        break;

    default:
        return QVariant();
        
    }
    if (Qt::SizeHintRole == role){
        QFontMetrics metrics(m_font);
        QSize s(metrics.width(value)+6, metrics.height());
        //qDebug() << "setting size of " << index.column() << "x" << index.row() << " (" << value << ") to " << s;
        return s;
    }
    if (message->isnew())
        value = "*" + value;
    return value;
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

    if (orientation != Qt::Horizontal)
        return QVariant();

    if (role == Qt::TextAlignmentRole)
        return Qt::AlignLeft;

    if (role == Qt::FontRole) {
        return m_font;
    }

    if (role == Qt::DisplayRole) {
        switch(section) {
        case COL_FOLDER:
            return QString("Folder");
        case COL_DATE:
            return QString("Date");
        case COL_FROM:
            return QString("From");
        case COL_SUBJECT:
            return QString("Subject");
        default:
            return QVariant();
        }
    }
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
    reset();
    emit dataChanged(createIndex(0,0,0),
                     createIndex(m_messages.count(),3,0));
    emit layoutChanged();
}

void IncomingMailModel::connectSignals(QTableView *mailView, QtIncoming *mainWidget)
{
    Q_UNUSED(mailView);
    Q_UNUSED(mainWidget);
}

#include "moc_incomingmailmodel.cpp"

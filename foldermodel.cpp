#include "foldermodel.h"
#include "ui_prefs.h"
#include "qtincoming.h"

#include <QSignalMapper>

#include <qdebug.h>

enum folder_columns 
{
    COL_FOLDERNAME    = 0,
    COL_NOTIFICATION  = 1,
};

folderModel::folderModel(QObject *parent, Ui::PrefWindow *prefui) :
    QAbstractTableModel(parent), m_theGrid(0), m_prefui(prefui)
{
}

QVariant folderModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::CheckStateRole &&
        index.row() < folders.count()) {
        if (index.column() == COL_NOTIFICATION) {
            if (folders[index.row()].doNotification())
                return Qt::Checked;
            else
                return Qt::Unchecked;
        }
        return QVariant();
    }

    if (role != Qt::DisplayRole && role != Qt::EditRole)
        return QVariant();

    if (index.row() >= folders.length())
        return QVariant();

    if (index.column() > 2)
        return QVariant();

    switch(index.column()) {

    case COL_FOLDERNAME:
        return folders[index.row()].folderName();

    case COL_NOTIFICATION:
        return  folders[index.row()].doNotification();

    default:
        return QString("");
    }
}

void folderModel::saveSettings(QSettings &settings)
{
    settings.beginWriteArray("folderList");
    for(int i = 0; i < widgets.count(); ++i) {
        QList<QWidget *> *row;
        QLineEdit *lineEdit;
        QCheckBox *checkBox;
        row = widgets[i];
        QString value;

        int columnNumber = 2;

        checkBox = dynamic_cast<QCheckBox *>((*row)[columnNumber++]);
        folders[i].set_doNotification(checkBox->isChecked());

        checkBox = dynamic_cast<QCheckBox *>((*row)[columnNumber++]);
        folders[i].set_doPopup(checkBox->isChecked());

#ifdef IS_MAEMO
        checkBox = dynamic_cast<QCheckBox *>((*row)[columnNumber++]);
        folders[i].set_doVibrate(checkBox->isChecked());

        checkBox = dynamic_cast<QCheckBox *>((*row)[columnNumber++]);
        folders[i].set_doLED(checkBox->isChecked());
#endif /* IS_MAEMO */

        lineEdit = dynamic_cast<QLineEdit *>((*row)[columnNumber++]);
        folders[i].set_folderName(value = lineEdit->text());

        lineEdit = dynamic_cast<QLineEdit *>((*row)[columnNumber++]);
        folders[i].set_displayName(value = lineEdit->text());

        settings.setArrayIndex(i);
        settings.setValue("folderName", folders[i].folderName());
        settings.setValue("displayName", folders[i].displayName());
        settings.setValue("folderNotification", folders[i].doNotification());
        settings.setValue("folderPopup", folders[i].doPopup());
        settings.setValue("folderVibrate", folders[i].doVibrate());
        settings.setValue("folderLED", folders[i].doLED());

    }
    settings.endArray();
}

void folderModel::readSettings(QSettings &settings)
{
    int size = settings.beginReadArray("folderList");
    folders.clear();
    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        if (settings.value("folderName").toString().length() == 0)
            continue; // don't save this empty folder name
        folders.push_back(
            folderItem(settings.value("folderName").toString(),
                       settings.value("displayName").toString(),
                       settings.value("folderNotification", false).toBool(),
                       settings.value("folderPopup", false).toBool(),
                       settings.value("folderVibrate", false).toBool(),
                       settings.value("folderLED", false).toBool()));
    }
    settings.endArray();
}

int folderModel::count() const
{
    return folders.count();
}

folderItem &folderModel::folder(int row) {
    return folders[row];
}

const QString &folderModel::displayName(int row) const
{
    return ((folders[row].displayName().length() > 0) ? folders[row].displayName() : folders[row].folderName());
}

const QString &folderModel::folderName(int row) const
{
    return folders[row].folderName();
}

bool folderModel::enablePopup(int row) const
{
    return folders[row].doNotification();
}

#include <iostream>

bool folderModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    std::cerr << "role: " << role << "\n";

    if (!index.isValid())
        return false;
    
    if (role == Qt::CheckStateRole && index.column() == COL_NOTIFICATION &&
        index.row() < folders.count()) {
        folders[index.row()].set_doNotification(value.toBool());
        return true;
    }
        
    if (role == Qt::EditRole) {
        QString tmpname;
        
        if (index.row() >= folders.count())
            insertRows(folders.count(), index.row()-folders.count()+1, index);
        switch (index.column()) {

        case COL_FOLDERNAME:
            tmpname = value.toString();
            folders[index.row()].set_folderName(tmpname);
            break;

        default:
            return false;
        }
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool folderModel::insertRows(int position, int rows, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    beginInsertRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        folders.insert(position, folderItem());
    }

    endInsertRows();
    return true;
}

bool folderModel::removeRows(int position, int rows, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row = 0; row < rows; ++row) {
        folders.removeAt(position);
    }

    endRemoveRows();
    return true;
}

int folderModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 20;
    //return folders.count();
}

int folderModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

folderItem &folderModel::getFolderAt(int row) {
    return folders[row];
}

/*
QVariant folderModel::headerData(int section, Qt::Orientation orientation, int role) const
{
}
*/

Qt::ItemFlags folderModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    switch(index.column()) {
    case COL_FOLDERNAME:
        return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsEditable;
    case COL_NOTIFICATION:
        return QAbstractItemModel::flags(index) | Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsUserCheckable;
    }

    return Qt::ItemIsEnabled;
}


void folderModel::setupFolderPrefs(int index) {
    qDebug() << "switched to tab " << index;

    if (index == 3) { // the index of the folder tab
        if (!m_theGrid) {
            m_theGrid = new QGridLayout();
            m_prefui->scrolledFolders->setLayout(m_theGrid);

            int columnNumber = 2;

            QLabel *header = new QLabel("N");
            header->setAlignment(Qt::AlignHCenter);
            m_theGrid->addWidget(header, 0, columnNumber++);

            header = new QLabel("P");
            header->setAlignment(Qt::AlignHCenter);
            m_theGrid->addWidget(header, 0, columnNumber++);

#ifdef IS_MAEMO
            header = new QLabel("V");
            header->setAlignment(Qt::AlignHCenter);
            m_theGrid->addWidget(header, 0, columnNumber++);

            header = new QLabel("L");
            header->setAlignment(Qt::AlignHCenter);
            m_theGrid->addWidget(header, 0, columnNumber++);
#endif /* IS_MAEMO */

            m_theGrid->addWidget(new QLabel("Folder"), 0, columnNumber++);
            m_theGrid->addWidget(new QLabel("Alias"), 0, columnNumber++);

            m_signalMapper = new QSignalMapper();
            connect(m_signalMapper, SIGNAL(mapped(int)), this, SLOT(moveFolder(int)));

            connect(m_prefui->addFolder, SIGNAL(clicked()), this, SLOT(addRow()));
        }
        for(int i = 1; i <= count(); i++) {
            QList<QWidget *> *row;
            QPushButton *button;
            QCheckBox *checkBox;
            QLineEdit *lineEdit;
            folderItem &folder = getFolderAt(i-1);

            int buttonNumber = 0;

            if (widgets.count() < i) {
                // a new row is needed
                row = new QList<QWidget *>();
                widgets.push_back(row);


                // Movement arrors
                QIcon icon(":/icons/arrow-up.png");
                button = new QPushButton(icon, "");
                row->push_back(button);
                m_theGrid->addWidget(button, i, buttonNumber++);
                connect(button, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
                m_signalMapper->setMapping(button, i*2);

                icon = QIcon(":/icons/arrow-down.png");
                button = new QPushButton(icon, "");
                row->push_back(button);
                m_theGrid->addWidget(button, i, buttonNumber++);
                connect(button, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
                m_signalMapper->setMapping(button, i*2+1);


                // Checkboxes for notification types
                checkBox = new QCheckBox();
                checkBox->setChecked(folder.doNotification());
                row->push_back(checkBox);
                m_theGrid->addWidget(checkBox, i, buttonNumber++);

                checkBox = new QCheckBox();
                checkBox->setChecked(folder.doPopup());
                row->push_back(checkBox);
                m_theGrid->addWidget(checkBox, i, buttonNumber++);

#ifdef IS_MAEMO
                checkBox = new QCheckBox();
                checkBox->setChecked(folder.doVibrate());
                row->push_back(checkBox);
                m_theGrid->addWidget(checkBox, i, buttonNumber++);

                checkBox = new QCheckBox();
                checkBox->setChecked(folder.doLED());
                row->push_back(checkBox);
                m_theGrid->addWidget(checkBox, i, buttonNumber++);
#endif /* IS_MAEMO */

                // folder editing
                lineEdit = new QLineEdit(folder.folderName());
                row->push_back(lineEdit);
                m_theGrid->addWidget(lineEdit, i, buttonNumber++);

                lineEdit = new QLineEdit(folder.displayName());
                row->push_back(lineEdit);
                m_theGrid->addWidget(lineEdit, i, buttonNumber++);
            } else {
                // change the exist widget settings
                //qDebug() << " changing settings for #" << i << " = " << folder.folderName();
                row = widgets[i-1];

                buttonNumber = 2;

                // checkBoxes
                checkBox = dynamic_cast<QCheckBox *>((*row)[buttonNumber++]);
                checkBox->setChecked(folder.doNotification());

                checkBox = dynamic_cast<QCheckBox *>((*row)[buttonNumber++]);
                checkBox->setChecked(folder.doPopup());

#ifdef IS_MAEMO
                checkBox = dynamic_cast<QCheckBox *>((*row)[buttonNumber++]);
                checkBox->setChecked(folder.doVibrate());

                checkBox = dynamic_cast<QCheckBox *>((*row)[buttonNumber++]);
                checkBox->setChecked(folder.doLED());
#endif /* IS_MAEMO */

                // folder editing
                lineEdit = dynamic_cast<QLineEdit *>((*row)[buttonNumber++]);
                lineEdit->setText(folder.folderName());

                lineEdit = dynamic_cast<QLineEdit *>((*row)[buttonNumber++]);
                lineEdit->setText(folder.displayName());
            }
        }
        m_gridCount = count();
    }
}

void folderModel::moveFolder(int folderNumber) {
    qDebug() << "moved: " << folderNumber;
    if (folderNumber % 2 == 0) {
        folderNumber = folderNumber / 2 - 1;
        folders.swap(folderNumber, folderNumber-1);
    } else {
        folderNumber = (folderNumber - 1) / 2 - 1;
        folders.swap(folderNumber, folderNumber+1);
    }
    setupFolderPrefs(3);
}

void folderModel::addRow() {
    folders.append(folderItem());
    setupFolderPrefs(3);
}

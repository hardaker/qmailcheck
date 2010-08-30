#include "foldermodel.h"
#include "ui_prefs.h"

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
    for(int i = 1; i <= widgets.count(); ++i) {
        QList<QWidget *> *row;
        QLineEdit *lineEdit;
        QCheckBox *checkBox;
        row = widgets[i];
        QString value;

        checkBox = dynamic_cast<QCheckBox *>((*row)[2]);
        folders[i].set_doNotification(checkBox->isChecked());

        checkBox = dynamic_cast<QCheckBox *>((*row)[3]);
        folders[i].set_doPopup(checkBox->isChecked());

        checkBox = dynamic_cast<QCheckBox *>((*row)[4]);
        folders[i].set_doVibrate(checkBox->isChecked());

        checkBox = dynamic_cast<QCheckBox *>((*row)[5]);
        folders[i].set_doLED(checkBox->isChecked());

        lineEdit = dynamic_cast<QLineEdit *>((*row)[6]);
        folders[i].set_folderName(value = lineEdit->text());

        lineEdit = dynamic_cast<QLineEdit *>((*row)[7]);
        folders[i].set_displayName(value = lineEdit->text());

        settings.setArrayIndex(i);
        settings.setValue("folderName", folders[i-1].folderName());
        settings.setValue("displayName", folders[i-1].displayName());
        settings.setValue("folderNotification", folders[i-1].doNotification());
        settings.setValue("folderPopup", folders[i-1].doPopup());
        settings.setValue("folderVibrate", folders[i-1].doVibrate());
        settings.setValue("folderLED", folders[i-1].doLED());

    }
    settings.endArray();
}

void folderModel::readSettings(QSettings &settings)
{
    int size = settings.beginReadArray("folderList");
    folders.clear();
    for(int i = 1; i <= size; ++i) {
        settings.setArrayIndex(i-1);
        folders.push_back(
            folderItem(settings.value("folderName").toString(),
                       settings.value("displayName").toString(),
                       settings.value("folderPopup").toBool(),
                       settings.value("folderVibrate").toBool(),
                       settings.value("folderLED").toBool()));
    }
    settings.endArray();
}

int folderModel::count() const
{
    return folders.count();
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

            m_theGrid->addWidget(new QLabel("N"), 0, 2);
            m_theGrid->addWidget(new QLabel("P"), 0, 3);
            m_theGrid->addWidget(new QLabel("V"), 0, 4);
            m_theGrid->addWidget(new QLabel("L"), 0, 5);
            m_theGrid->addWidget(new QLabel("Folder"), 0, 6);
            m_theGrid->addWidget(new QLabel("Alias"), 0, 7);

            m_signalMapper = new QSignalMapper();
            connect(m_signalMapper, SIGNAL(mapped(int)), this, SLOT(moveFolder(int)));
        }
        for(int i = 1; i <= count(); i++) {
            QList<QWidget *> *row;
            QPushButton *button;
            QCheckBox *checkBox;
            QLineEdit *lineEdit;
            folderItem &folder = getFolderAt(i-1);

            if (widgets.count() < i) {
                // a new row is needed
                row = new QList<QWidget *>();
                widgets.push_back(row);

                button = new QPushButton("^");
                row->push_back(button);
                m_theGrid->addWidget(button, i, 0);
                connect(button, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
                m_signalMapper->setMapping(button, i*2);

                button = new QPushButton("v");
                row->push_back(button);
                m_theGrid->addWidget(button, i, 1);
                connect(button, SIGNAL(clicked()), m_signalMapper, SLOT(map()));
                m_signalMapper->setMapping(button, i*2+1);

                checkBox = new QCheckBox();
                checkBox->setChecked(folder.doNotification());
                row->push_back(checkBox);
                m_theGrid->addWidget(checkBox, i, 2);

                checkBox = new QCheckBox();
                checkBox->setChecked(folder.doPopup());
                row->push_back(checkBox);
                m_theGrid->addWidget(checkBox, i, 3);


                checkBox = new QCheckBox();
                checkBox->setChecked(folder.doVibrate());
                row->push_back(checkBox);
                m_theGrid->addWidget(checkBox, i, 4);


                checkBox = new QCheckBox();
                checkBox->setChecked(folder.doLED());
                row->push_back(checkBox);
                m_theGrid->addWidget(checkBox, i, 5);

                lineEdit = new QLineEdit(folder.folderName());
                row->push_back(lineEdit);
                m_theGrid->addWidget(lineEdit, i, 6);

                lineEdit = new QLineEdit(folder.displayName());
                row->push_back(lineEdit);
                m_theGrid->addWidget(lineEdit, i, 7);
            } else {
                // change the exist widget settings
                qDebug() << " changing settings for #" << i << " = " << folder.folderName();
                row = widgets[i-1];

                checkBox = dynamic_cast<QCheckBox *>((*row)[2]);
                checkBox->setChecked(folder.doNotification());

                checkBox = dynamic_cast<QCheckBox *>((*row)[3]);
                checkBox->setChecked(folder.doPopup());

                checkBox = dynamic_cast<QCheckBox *>((*row)[4]);
                checkBox->setChecked(folder.doVibrate());

                checkBox = dynamic_cast<QCheckBox *>((*row)[5]);
                checkBox->setChecked(folder.doLED());

                lineEdit = dynamic_cast<QLineEdit *>((*row)[6]);
                lineEdit->setText(folder.folderName());

                lineEdit = dynamic_cast<QLineEdit *>((*row)[7]);
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

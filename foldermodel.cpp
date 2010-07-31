#include "foldermodel.h"
#include "ui_prefs.h"

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
    for(int i = 0; i < folders.count(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("folderName", folders[i].folderName());
        settings.setValue("folderPopup", folders[i].doNotification());
    }
    settings.endArray();
}

void folderModel::readSettings(QSettings &settings)
{
    int size = settings.beginReadArray("folderList");
    folders.clear();
    for(int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        folders.push_back(
            folderItem(settings.value("folderName").toString(),
                       settings.value("folderName").toString(),
                       settings.value("folderPopup").toBool()));
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
        }
        for(int i = 0; i < count(); i++) {
            folderItem &folder = getFolderAt(i);

            if (widgets.count() >= i+1) {
                // should be recreating this
                // m_theGrid->removeWidget(widgets.at(i));
                // XXX: delete it
            } else {
                QList<QWidget *> *row = new QList<QWidget *>();
                widgets.push_back(row);

                QPushButton *button;
                button = new QPushButton("^");
                row->push_back(button);
                m_theGrid->addWidget(button, i, 0);

                button = new QPushButton("v");
                row->push_back(button);
                m_theGrid->addWidget(button, i, 1);

                QLineEdit *lineEdit;
                lineEdit = new QLineEdit(folder.folderName());
                row->push_back(lineEdit);
                m_theGrid->addWidget(lineEdit, i, 2);

                lineEdit = new QLineEdit(folder.displayName());
                row->push_back(lineEdit);
                m_theGrid->addWidget(lineEdit, i, 3);
            }
        }
    }
}

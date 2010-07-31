#include "foldermodel.h"

enum folder_columns 
{
    COL_FOLDERNAME    = 0,
    COL_NOTIFICATION  = 1,
};

folderModel::folderModel(QObject *parent) :
    QAbstractTableModel(parent)
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
    return folders.at(row);
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

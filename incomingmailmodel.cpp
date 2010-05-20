#include "incomingmailmodel.h"

#include <QtGui/QLabel>

IncomingMailModel::IncomingMailModel(QObject *parent) :
    QAbstractTableModel(parent), mailInfo()
{
    mailInfo.push_back("foo");
}

int IncomingMailModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

int IncomingMailModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 4;
}

Qt::ItemFlags IncomingMailModel::flags(const QModelIndex &index) const
{
    return 0;
}

QVariant IncomingMailModel::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(index);
    return QString::number(index.row() * index.column());
}

QVariant IncomingMailModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg("section");
    else
        return QString("Row %1").arg("section");
}

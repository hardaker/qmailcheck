#ifndef FOLDERMODEL_H
#define FOLDERMODEL_H

#include <QAbstractTableModel>
#include <QSettings>

#include "folderitem.h"

class folderModel : public QAbstractTableModel
{
Q_OBJECT
public:
    explicit folderModel(QObject *parent = 0);

    QVariant data(const QModelIndex &index, int role) const;
    const QString &folderName(int row) const;
    bool    enablePopup(int row) const;
    int count() const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    bool insertRows(int position, int rows, const QModelIndex &parent);
    bool removeRows(int position, int rows, const QModelIndex &parent);
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    void saveSettings(QSettings &settings);
    void readSettings(QSettings &settings);
    // QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    folderItem &getFolderAt(int row);

signals:

public slots:

private:
    QList<folderItem> folders;

};

#endif // FOLDERMODEL_H

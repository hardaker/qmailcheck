#ifndef INCOMINGMAILMODEL_H
#define INCOMINGMAILMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class IncomingMailModel : public QAbstractTableModel
{
Q_OBJECT
public:
    explicit IncomingMailModel(QObject *parent = 0);
    QVariant data(const QModelIndex &index, int role) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
signals:

public slots:

private:
    QStringList mailInfo;

};

#endif // INCOMINGMAILMODEL_H

#ifndef FOLDERMODEL_H
#define FOLDERMODEL_H

#include <QAbstractTableModel>
#include <QSettings>
#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtCore/QSignalMapper>
#include <QtGui/QPushButton>

#include "folderitem.h"

namespace Ui {
    class QtIncoming;
    class PrefWindow;
}

class folderModel : public QAbstractTableModel
{
Q_OBJECT
public:
    explicit folderModel(QObject *parent, Ui::PrefWindow *prefui);

    QVariant data(const QModelIndex &index, int role) const;
    const QString &folderName(int row) const;
    const QString &displayName(int row) const;
    folderItem &folder(int row);
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
    void setupFolderPrefs(int index);
    void moveFolder(int folderNumber);
    void addRow();

private:
    QList<folderItem> folders;
    QList<QList<QWidget *> *>    widgets;
    QGridLayout*       m_theGrid;
    Ui::PrefWindow    *m_prefui;
    int                m_gridCount;
    QSignalMapper     *m_signalMapper;
    QPushButton       *m_addButton;
};

#endif // FOLDERMODEL_H

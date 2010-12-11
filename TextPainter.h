#ifndef TEXTPAINTER_H
#define TEXTPAINTER_H

#include <QStyledItemDelegate>

class TextPainter : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit TextPainter(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option,
                   const QModelIndex &index) const;

signals:

public slots:

};

#endif // TEXTPAINTER_H

#include "TextPainter.h"
#include <QtGui/QPainter>
#include <QApplication>

TextPainter::TextPainter(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void TextPainter::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    QColor expectedColor = index.data(Qt::ForegroundRole).value<QColor>();

    painter->save();
    QBrush brush = option.palette.foreground();

    QString value = index.data().toString();
    if (value[0] == '*') {
        brush.setColor(QColor(220,220,255));
        value.remove(0, 1);
    } else
        brush.setColor(Qt::white);

    painter->setBrush(brush);
    painter->fillRect(option.rect.adjusted(-1,-1,1,1), brush);

    brush.setColor(Qt::black);
    painter->setBrush(brush);
    painter->drawText(option.rect, index.data().toString());
//    painter->drawLine(option.rect.topLeft(), option.rect.bottomRight());
    painter->restore();
}


QSize TextPainter::sizeHint(const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    return QStyledItemDelegate::sizeHint(option, index);
}

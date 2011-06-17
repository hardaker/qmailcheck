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
#if defined(Q_WS_MAEMO_5) || defined(MAEMO_CHANGES)
        brush.setColor(QColor(0,0,50));
#else
        brush.setColor(QColor(220,220,255));
#endif
        value.remove(0, 1);
    } else
#if defined(Q_WS_MAEMO_5) || defined(MAEMO_CHANGES)
        brush.setColor(Qt::black);
#else
        brush.setColor(Qt::white);
#endif

    painter->setBrush(brush);
    painter->fillRect(option.rect.adjusted(-1,-1,1,1), brush);

#if defined(Q_WS_MAEMO_5) || defined(MAEMO_CHANGES)
    brush.setColor(Qt::white);
#else
    brush.setColor(Qt::black);
#endif

    painter->setBrush(brush);
    painter->setFont(index.data(Qt::FontRole).value<QFont>());
    painter->drawText(option.rect, value);
//    painter->drawLine(option.rect.topLeft(), option.rect.bottomRight());
    painter->restore();
}


QSize TextPainter::sizeHint(const QStyleOptionViewItem &option,
                            const QModelIndex &index) const
{
    QSize s(option.fontMetrics.size(Qt::TextSingleLine, index.data().toString()));
    if (QApplication::style()->inherits("QMaemo5Style")) {
        if (s.height() < 70) {
            s.setHeight(70); // MAEMO hack; ugh.
        }
    }
    return s;
}

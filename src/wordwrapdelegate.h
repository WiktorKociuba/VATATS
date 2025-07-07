#pragma once
#include <QStyledItemDelegate>
#include <QTreeWidget>
#include <QApplication>
#include <QTextDocument>

class WordWrapDelegate : public QStyledItemDelegate{
public:
    using QStyledItemDelegate::QStyledItemDelegate;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QStyleOptionViewItem options = option;
        initStyleOption(&options, index);
        options.textElideMode = Qt::ElideNone;
        options.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
        options.features |= QStyleOptionViewItem::WrapText;
        const QWidget *widget = option.widget;
        QStyle *style = widget ? widget->style() : QApplication::style();
        style->drawControl(QStyle::CE_ItemViewItem, &options, painter, widget);
    }
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override{
        QStyleOptionViewItem options = option;
        initStyleOption(&options, index);
        options.textElideMode = Qt::ElideNone;
        options.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;
        QTextDocument doc;
        doc.setDefaultFont(options.font);
        int width = options.rect.width();
        if(const QTreeView* view = qobject_cast<const QTreeView*>(options.widget)){
            width = view->columnWidth(index.column());
        }
        doc.setTextWidth(width > 0 ? width : 200);
        doc.setPlainText(options.text);
        return QSize(doc.idealWidth(), doc.size().height());
    }
};
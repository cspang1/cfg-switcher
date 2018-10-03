#ifndef CHECKBOXDELEGATE_H
#define CHECKBOXDELEGATE_H

#include <QStyledItemDelegate>

class CheckboxDelegate : public QStyledItemDelegate
{
        public:
            CheckboxDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}
            void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
            virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
        };

#endif // CHECKBOXDELEGATE_H

#ifndef GameDelegate_H
#define GameDelegate_H

#include <QStyledItemDelegate>

class GameDelegate : public QStyledItemDelegate
{
        public:
            GameDelegate(QObject *parent = nullptr) : QStyledItemDelegate(parent) {}
            void paint ( QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index ) const;
            virtual bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);
        };

#endif // GameDelegate_H

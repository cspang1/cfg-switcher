#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include "gamedelegate.h"

void GameDelegate::paint (QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const {
    Q_ASSERT(index.isValid());

    QStyleOptionViewItem viewItemOption(option);
    if (index.column() == 0) {
        const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
        QRect newRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                            QSize(option.decorationSize.width() + 5,option.decorationSize.height()),
                                            QRect(option.rect.x() + textMargin, option.rect.y(),
                                                  option.rect.width() - (2 * textMargin), option.rect.height()));
        viewItemOption.rect = newRect;
        QStyledItemDelegate::paint(painter, viewItemOption, index);
    }
    else if(index.column() > 2) {
        QPixmap pixmap = qvariant_cast<QPixmap>(index.model()->data(index, Qt::DecorationRole));
        pixmap = pixmap.scaled(QSize(24, 24), Qt::KeepAspectRatio);;

       // Position our pixmap
       const int x = option.rect.center().x() - pixmap.rect().width() / 2;
       const int y = option.rect.center().y() - pixmap.rect().height() / 2;

       if (option.state & QStyle::State_Selected) {
           painter->fillRect(option.rect, option.palette.highlight());
       }

       painter->drawPixmap(QRect(x, y, pixmap.rect().width(), pixmap.rect().height()), pixmap);
    }
    else
        QStyledItemDelegate::paint(painter, viewItemOption, index);
}

bool GameDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) {
    Q_ASSERT(event);
    Q_ASSERT(model);

    // make sure that the item is checkable
    Qt::ItemFlags flags = model->flags(index);
    if (!(flags & Qt::ItemIsUserCheckable) || !(flags & Qt::ItemIsEnabled))
        return false;
    // make sure that we have a check state
    QVariant value = index.data(Qt::CheckStateRole);
    if (!value.isValid())
        return false;
    // make sure that we have the right event type
    if (event->type() == QEvent::MouseButtonRelease) {
        const int textMargin = QApplication::style()->pixelMetric(QStyle::PM_FocusFrameHMargin) + 1;
        QRect checkRect = QStyle::alignedRect(option.direction, Qt::AlignCenter,
                                              option.decorationSize,
                                              QRect(option.rect.x() + (2 * textMargin), option.rect.y(),
                                                    option.rect.width() - (2 * textMargin),
                                                    option.rect.height()));
        if (!checkRect.contains(static_cast<QMouseEvent*>(event)->pos()))
            return false;
    } else if (event->type() == QEvent::KeyPress) {
        if (static_cast<QKeyEvent*>(event)->key() != Qt::Key_Space&& static_cast<QKeyEvent*>(event)->key() != Qt::Key_Select)
            return false;
    } else {
        return false;
    }
    Qt::CheckState state = (static_cast<Qt::CheckState>(value.toInt()) == Qt::Checked
                            ? Qt::Unchecked : Qt::Checked);
    return model->setData(index, state, Qt::CheckStateRole);
}

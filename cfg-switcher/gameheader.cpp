#include "gameheader.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QOperatingSystemVersion>
#include <QPainter>

GameHeader::GameHeader(Qt::Orientation orientation, QWidget* parent)
    : QHeaderView(orientation, parent), isChecked_(false)
{
    if(QOperatingSystemVersion::current() >= QOperatingSystemVersion::Windows10){
        setStyleSheet(
            "QHeaderView::section{"
                "border-top:0px solid #D8D8D8;"
                "border-left:0px solid #D8D8D8;"
                "border-right:1px solid #D8D8D8;"
                "border-bottom: 1px solid #D8D8D8;"
                "background-color:white;"
                "padding:4px;"
            "}"
            "QTableCornerButton::section{"
                "border-top:0px solid #D8D8D8;"
                "border-left:0px solid #D8D8D8;"
                "border-right:1px solid #D8D8D8;"
                "border-bottom: 1px solid #D8D8D8;"
                "background-color:white;"
            "}");}
}

void GameHeader::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();
    QStyleOptionButton option;
    switch(logicalIndex) {
        case 0:
            option.rect = QRect(3,10,16,16);
            option.state = QStyle::State_Enabled | QStyle::State_Active;
            if (isChecked_)
                option.state |= QStyle::State_On;
            else
                option.state |= QStyle::State_Off;
            option.state |= QStyle::State_Off;
            style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);
            break;
        case 1:
            painter->drawText(rect, Qt::AlignCenter, "Game ID");
            break;
        case 2:
            painter->drawText(rect, Qt::AlignCenter, "Config Path");
            break;
        case 3:
            painter->drawText(rect, Qt::AlignCenter, "Main Config Set");
            break;
        case 4:
            painter->drawText(rect, Qt::AlignCenter, "Battery Config Set");
            break;
        case 5:
            painter->drawText(rect, Qt::AlignCenter, "Enabled");
    }

}

void GameHeader::mouseReleaseEvent(QMouseEvent* event)
{
    if(logicalIndexAt(event->pos()) == 0) {
        setIsChecked(!isChecked());
        if(isChecked())
            emit checkBoxClicked(Qt::Checked);
        else
            emit checkBoxClicked(Qt::Unchecked);
    }
}

void GameHeader::redrawCheckBox()
{
    viewport()->update();
}

void GameHeader::setIsChecked(bool val)
{
    if (isChecked_ != val)
    {
        isChecked_ = val;

        redrawCheckBox();
    }
}

void GameHeader::setSelectAll(bool state) {
    setIsChecked(state);
}

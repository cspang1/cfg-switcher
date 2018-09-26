#include "checkboxheader.h"
#include <QPainter>

CheckboxHeader::CheckboxHeader(Qt::Orientation orientation, QWidget* parent /*= 0*/)
    : QHeaderView(orientation, parent)
{
    isChecked_ = false;
}

void CheckboxHeader::paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const
{
    painter->save();
    QHeaderView::paintSection(painter, rect, logicalIndex);
    painter->restore();
    if (logicalIndex == 0)
    {
        QStyleOptionButton option;

        option.rect = QRect(3,10,16,16);

        option.state = QStyle::State_Enabled | QStyle::State_Active;

        if (isChecked_)
            option.state |= QStyle::State_On;
        else
            option.state |= QStyle::State_Off;
        option.state |= QStyle::State_Off;

        style()->drawPrimitive(QStyle::PE_IndicatorCheckBox, &option, painter);
    }
}

void CheckboxHeader::mouseReleaseEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    setIsChecked(!isChecked());
    if(isChecked())
        emit checkBoxClicked(Qt::Checked);
    else
        emit checkBoxClicked(Qt::Unchecked);
}

void CheckboxHeader::redrawCheckBox()
{
    viewport()->update();
}

void CheckboxHeader::setIsChecked(bool val)
{
    if (isChecked_ != val)
    {
        isChecked_ = val;

        redrawCheckBox();
    }
}

void CheckboxHeader::setSelectAll(bool state) {
    setIsChecked(state);
}

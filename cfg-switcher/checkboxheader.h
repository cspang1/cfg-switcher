#ifndef CHECKBOXHEADER_H
#define CHECKBOXHEADER_H

#include <QHeaderView>

class CheckboxHeader : public QHeaderView
{
    Q_OBJECT

public:
    CheckboxHeader(Qt::Orientation orientation, QWidget* parent = nullptr);
    bool isChecked() const { return isChecked_; }
    void setIsChecked(bool val);

signals:
    void checkBoxClicked(Qt::CheckState state);

public slots:
    void setSelectAll(bool state);

protected:
    void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const;
    void mouseReleaseEvent(QMouseEvent* event);

private:
    bool isChecked_;
    void redrawCheckBox();
};

#endif // CHECKBOXHEADER_H

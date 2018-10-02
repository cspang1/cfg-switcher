#ifndef GameHeader_H
#define GameHeader_H

#include <QHeaderView>

class GameHeader : public QHeaderView
{
    Q_OBJECT

public:
    GameHeader(Qt::Orientation orientation, QWidget* parent = nullptr);
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

#endif // GameHeader_H

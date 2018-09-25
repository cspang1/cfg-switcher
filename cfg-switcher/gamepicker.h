#ifndef GAMEPICKER_H
#define GAMEPICKER_H

#include <QDialog>

namespace Ui {
class GamePicker;
}

class GamePicker : public QDialog
{
    Q_OBJECT

public:
    explicit GamePicker(QWidget *parent = nullptr);
    ~GamePicker();
    QString getGameName();
    QString getGamePath();

private slots:
    void on_cancelAddGameBtn_clicked();
    void on_browseGameBtn_clicked();
    void on_saveGameBtn_clicked();

private:
    Ui::GamePicker *ui;
};

#endif // GAMEPICKER_H

#ifndef CFGSWITCHER_H
#define CFGSWITCHER_H

#include <QWidget>
#include <QAbstractNativeEventFilter>
#include <Windows.h>
#include "settings.h"
#include "gamemodel.h"
#include "checkboxheader.h"

namespace Ui {
class CfgSwitcher;
}

class CfgSwitcher : public QWidget, public QAbstractNativeEventFilter
{
    Q_OBJECT
private:
    GameModel gameModel;
    Settings settings;
    Ui::CfgSwitcher *ui;
    PowerState CurrentACStatus;
    PowerState getPowerState();
    void setPowerStatusLabel();

public:
    explicit CfgSwitcher(QWidget *parent = nullptr);
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE;
    ~CfgSwitcher() override;
    void addGame(Game game);
    void removeGame(QString gameName);
    bool switchConfigs(PowerState pState, Game &game);
    bool switchConfigs(PowerState pState);
    bool switchConfigs();
    void setConfigs(PowerState pState);
    void setStatus(bool status);    

public slots:
    void setGameBtns(bool state);

private slots:
    void on_setMainCfgBtn_clicked();
    void on_setBattCfgBtn_clicked();
    void on_quitButton_clicked();
    void on_addGameBtn_clicked();
    void on_remGames_clicked();
    void on_enableBtn_clicked();
    void on_disableBtn_clicked();
};

#endif // CFGSWITCHER_H

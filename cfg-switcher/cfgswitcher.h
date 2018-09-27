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
    BYTE CurrentACStatus;
    BYTE getPowerStatus();
    void setPowerStatusLabel();

public:
    explicit CfgSwitcher(QWidget *parent = nullptr);
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE;
    ~CfgSwitcher() override;
    void addGame(QString gameName, QString gamePath, bool mainCfgSet, bool battCfgSet);
    void removeGame(QString gameName);
    bool switchConfigs(int pState, Game &game);
    bool switchConfigs(int pState);
    bool setConfigs(int pState);

public slots:
    void setGameBtns(bool state);

private slots:
    void on_setMainCfgBtn_clicked();
    void on_setBattCfgBtn_clicked();
    void on_quitButton_clicked();
    void on_addGameBtn_clicked();
    void on_remGames_clicked();
};

#endif // CFGSWITCHER_H

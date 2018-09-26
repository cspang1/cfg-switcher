#ifndef CFGSWITCHER_H
#define CFGSWITCHER_H

#include <QWidget>
#include <QAbstractNativeEventFilter>
#include <Windows.h>
#include "settings.h"
#include "gamemodel.h"

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
    void addGame(QString gameName, QString gamePath);
    bool switchConfigs(int pState, Settings &settings, game &game);
    bool switchConfigs(int pState, Settings &settings);

private slots:
    void on_setMainCfgBtn_clicked();
    void on_setBattCfgBtn_clicked();
    void on_quitButton_clicked();
    void on_addGameBtn_clicked();
};

#endif // CFGSWITCHER_H

#ifndef CFGSWITCHER_H
#define CFGSWITCHER_H

#include <QWidget>
#include <QAbstractNativeEventFilter>
#include <Windows.h>
#include "Settings.h"

namespace Ui {
class CfgSwitcher;
}

class CfgSwitcher : public QWidget, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit CfgSwitcher(QWidget *parent = nullptr);
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE;
    ~CfgSwitcher();

private slots:
    void on_setMainCfgBtn_clicked();
    void on_setBattCfgBtn_clicked();

    void on_quitButton_clicked();

private:
    Settings settings;
    Ui::CfgSwitcher *ui;
    BYTE CurrentACStatus;
    BYTE getPowerStatus();
    void setPowerStatusLabel();
};

#endif // CFGSWITCHER_H

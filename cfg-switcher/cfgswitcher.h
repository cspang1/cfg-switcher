#ifndef CFGSWITCHER_H
#define CFGSWITCHER_H

#include <QObject>
#include <QAbstractNativeEventFilter>
#include <Windows.h>

class CfgSwitcher : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
private:
    BYTE CurrentACStatus;
    BYTE getPowerStatus();

public:
    explicit CfgSwitcher(QObject *parent = nullptr);
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE;

signals:

public slots:
};

#endif // CFGSWITCHER_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractNativeEventFilter>
#include <QMainWindow>
#include <QDebug>
#include <Windows.h>
#include <QMessageBox>

class MainWindow : public QMainWindow, public QAbstractNativeEventFilter
{
    Q_OBJECT
private:
    BYTE CurrentACStatus;
    BYTE getPowerStatus();

public:
    explicit MainWindow(QWidget *parent = nullptr);
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *) Q_DECL_OVERRIDE
    {
        QMessageBox Msgbox;
        BYTE ACLineStatus = getPowerStatus();
        bool ACStatusChanged = false;
        MSG *msg = static_cast< MSG * >( message );
        if(msg->message == WM_POWERBROADCAST) {
            ACLineStatus = getPowerStatus();
            ACStatusChanged = CurrentACStatus != ACLineStatus;
            CurrentACStatus = ACStatusChanged ? ACLineStatus : CurrentACStatus;
            if (ACStatusChanged) {
                switch (CurrentACStatus) {
                    case 0:
                        Msgbox.setText("UNPLUGGED");
                        break;
                    case 1:
                        Msgbox.setText("PLUGGED IN");
                        break;
                    default:
                        Msgbox.setText("Ummmm...");
                        //std::cerr << "Error: Invalid AC line status - " << GetLastErrorAsString() << std::endl;
                        break;
                }
                Msgbox.exec();
            }
        }
        return false;
    }

signals:

public slots:
};

#endif // MAINWINDOW_H

#ifndef HOSTWINDOW_H
#define HOSTWINDOW_H

#include <vector>
#include <QMainWindow>
#include <QVector>
#include <QTimer>

#include "adl_device.h"

namespace Ui {
class HostWindow;
}

class HostWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HostWindow(QWidget *parent = 0);
    ~HostWindow();

    QVector<ADL_device*> device;

public slots:
    void tryChangeDevice();
    void changeDevice( int newDevice );
    void updateInfo();

    void trySetGPUClock( int newClock );
    void trySetMemClock( int newClock );

private:
    Ui::HostWindow *ui;

    QTimer *updateTimer;

    int currentDevice;
};

#endif // HOSTWINDOW_H

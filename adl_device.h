#ifndef ADL_DEVICE_H
#define ADL_DEVICE_H

#include "adl/adl_sdk.h"
#include <QString>
#include <Windows.h>

class ADL_device
{
public:
    ADL_device( AdapterInfo device );

    static int getDeviceCount();
    static HINSTANCE gethDLL();
    int adapterId;
    int deviceId;
    QString adapterName,displayName;

    int getGPUClock();
    int getGPUClockStep();
    int getGPUClockMax();
    int getGPUClockMin();
    int getMemClock();
    int getMemClockStep();
    int getMemClockMax();
    int getMemClockMin();
    int getHeat();
    int getFan();
    int getFanMax();

    int getVoltage();
    int getVoltageStep();
    int getVoltageMax();

    void setGPUClock( int newClock );
    void setMemClock( int newClock );

    void   upGPUClock();
    void downGPUClock();

    void   upMemClock();
    void downMemClock();
};

#endif // ADL_DEVICE_H

#include "adl_device.h"
#include "adl_define.h"

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>

bool ADLMainControl_ready = false;

HINSTANCE hDLL;

ADL_OVERDRIVE6_FANSPEED_GET ADL_Overdrive6_FanSpeed_Get;
ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS ADL_Overdrive6_ThermalController_Caps;
ADL_OVERDRIVE6_TEMPERATURE_GET ADL_Overdrive6_Temperature_Get;
ADL_OVERDRIVE6_CAPABILITIES_GET ADL_Overdrive6_Capabilities_Get;
ADL_OVERDRIVE6_STATEINFO_GET ADL_Overdrive6_StateInfo_Get;
ADL_OVERDRIVE6_CURRENTSTATUS_GET ADL_Overdrive6_CurrentStatus_Get;
ADL_OVERDRIVE6_POWERCONTROL_CAPS ADL_Overdrive6_PowerControl_Caps;
ADL_OVERDRIVE6_POWERCONTROLINFO_GET ADL_Overdrive6_PowerControlInfo_Get;
ADL_OVERDRIVE6_POWERCONTROL_GET ADL_Overdrive6_PowerControl_Get;
ADL_OVERDRIVE6_FANSPEED_SET ADL_Overdrive6_FanSpeed_Set;
ADL_OVERDRIVE6_STATE_SET ADL_Overdrive6_State_Set;
ADL_OVERDRIVE6_POWERCONTROL_SET ADL_Overdrive6_PowerControl_Set;

void* __stdcall ADL_Main_Memory_Alloc ( int iSize )
{
    void* lpBuffer = malloc ( iSize );
    return lpBuffer;
}

// Optional Memory de-allocation function
void __stdcall ADL_Main_Memory_Free ( void** lpBuffer )
{
    if ( NULL != *lpBuffer )
    {
        free ( *lpBuffer );
        *lpBuffer = NULL;
    }
}

void initADLMainControl()
{
    if( ADLMainControl_ready )
    {
        return;
    }

    ADL_MAIN_CONTROL_CREATE ADL_Main_Control_Create;

    hDLL = LoadLibrary(L"atiadlxx.dll");
    if (hDLL == NULL)
    {
        printf("Cannot get DLL!\n");
        return;
    }

    ADL_Main_Control_Create = (ADL_MAIN_CONTROL_CREATE) GetProcAddress(hDLL,"ADL_Main_Control_Create");

    if ( ADL_OK != ADL_Main_Control_Create (ADL_Main_Memory_Alloc, 1) )
    {
        printf("ADL Initialization Error!\n");
        return;
    }

    ADL_Overdrive6_FanSpeed_Get = (ADL_OVERDRIVE6_FANSPEED_GET) GetProcAddress(hDLL,"ADL_Overdrive6_FanSpeed_Get");
    ADL_Overdrive6_ThermalController_Caps = (ADL_OVERDRIVE6_THERMALCONTROLLER_CAPS)GetProcAddress (hDLL, "ADL_Overdrive6_ThermalController_Caps");
    ADL_Overdrive6_Temperature_Get = (ADL_OVERDRIVE6_TEMPERATURE_GET)GetProcAddress (hDLL, "ADL_Overdrive6_Temperature_Get");
    ADL_Overdrive6_Capabilities_Get = (ADL_OVERDRIVE6_CAPABILITIES_GET)GetProcAddress(hDLL, "ADL_Overdrive6_Capabilities_Get");
    ADL_Overdrive6_StateInfo_Get = (ADL_OVERDRIVE6_STATEINFO_GET)GetProcAddress(hDLL, "ADL_Overdrive6_StateInfo_Get");
    ADL_Overdrive6_CurrentStatus_Get = (ADL_OVERDRIVE6_CURRENTSTATUS_GET)GetProcAddress(hDLL, "ADL_Overdrive6_CurrentStatus_Get");
    ADL_Overdrive6_PowerControl_Caps = (ADL_OVERDRIVE6_POWERCONTROL_CAPS)GetProcAddress(hDLL, "ADL_Overdrive6_PowerControl_Caps");
    ADL_Overdrive6_PowerControlInfo_Get = (ADL_OVERDRIVE6_POWERCONTROLINFO_GET)GetProcAddress(hDLL, "ADL_Overdrive6_PowerControlInfo_Get");
    ADL_Overdrive6_PowerControl_Get = (ADL_OVERDRIVE6_POWERCONTROL_GET)GetProcAddress(hDLL, "ADL_Overdrive6_PowerControl_Get");
    ADL_Overdrive6_FanSpeed_Set  = (ADL_OVERDRIVE6_FANSPEED_SET)GetProcAddress(hDLL, "ADL_Overdrive6_FanSpeed_Set");
    ADL_Overdrive6_State_Set = (ADL_OVERDRIVE6_STATE_SET)GetProcAddress(hDLL, "ADL_Overdrive6_State_Set");
    ADL_Overdrive6_PowerControl_Set = (ADL_OVERDRIVE6_POWERCONTROL_SET) GetProcAddress(hDLL, "ADL_Overdrive6_PowerControl_Set");

    if (NULL == ADL_Overdrive6_FanSpeed_Get ||
            NULL == ADL_Overdrive6_ThermalController_Caps ||
            NULL == ADL_Overdrive6_Temperature_Get ||
            NULL == ADL_Overdrive6_Capabilities_Get ||
            NULL == ADL_Overdrive6_StateInfo_Get ||
            NULL == ADL_Overdrive6_CurrentStatus_Get ||
            NULL == ADL_Overdrive6_PowerControl_Caps ||
            NULL == ADL_Overdrive6_PowerControlInfo_Get ||
            NULL == ADL_Overdrive6_PowerControl_Get ||
            NULL == ADL_Overdrive6_FanSpeed_Set ||
            NULL == ADL_Overdrive6_State_Set ||
            NULL == ADL_Overdrive6_PowerControl_Set)
    {
        printf("ADL's API is missing!\n");
        return;
    }

    ADLMainControl_ready = true;
}

HINSTANCE ADL_device::gethDLL()
{
    return hDLL;
}

int ADL_device::getDeviceCount()
{
    initADLMainControl();

    ADL_ADAPTER_NUMBEROFADAPTERS_GET ADL_Adapter_NumberOfAdapters_Get;
    ADL_Adapter_NumberOfAdapters_Get = (ADL_ADAPTER_NUMBEROFADAPTERS_GET) GetProcAddress(hDLL,"ADL_Adapter_NumberOfAdapters_Get");

    int  iNumberAdapters = 0;

    if ( ADL_OK != ADL_Adapter_NumberOfAdapters_Get ( &iNumberAdapters ) )
    {
        printf("Cannot get the number of adapters!\n");
        return 0;
    }

    return iNumberAdapters;
}

ADL_device::ADL_device( AdapterInfo device )
{
    this->deviceId = device.iDeviceNumber;
    this->adapterId = device.iAdapterIndex;

    this->adapterName = device.strAdapterName;
    this->displayName = device.strDisplayName;
}

int ADL_device::getGPUClock()
{
    ADLOD6CurrentStatus currentStatus = {0};

    if (ADL_OK != ADL_Overdrive6_CurrentStatus_Get (this->adapterId, &currentStatus))
    {
        printf("Failed to get custom performance levels info\n");
        return 0;
    }

    return (currentStatus.iEngineClock/100);
}

int ADL_device::getGPUClockMax()
{
    ADLOD6Capabilities od6Capabilities = {0};

    if (ADL_OK != ADL_Overdrive6_Capabilities_Get (this->adapterId, &od6Capabilities))
    {
        printf("Failed to get Overdrive capabilities\n");
        return 0;
    }

    return (od6Capabilities.sEngineClockRange.iMax / 100);
}

int ADL_device::getGPUClockMin()
{
    ADLOD6Capabilities od6Capabilities = {0};

    if (ADL_OK != ADL_Overdrive6_Capabilities_Get (this->adapterId, &od6Capabilities))
    {
        printf("Failed to get Overdrive capabilities\n");
        return 0;
    }

    return (od6Capabilities.sEngineClockRange.iMin / 100);
}

int ADL_device::getGPUClockStep()
{
    ADLOD6Capabilities od6Capabilities = {0};

    if (ADL_OK != ADL_Overdrive6_Capabilities_Get (this->adapterId, &od6Capabilities))
    {
        printf("Failed to get Overdrive capabilities\n");
        return 0;
    }

    return (od6Capabilities.sEngineClockRange.iStep / 100);
}

int ADL_device::getMemClock()
{
    ADLOD6CurrentStatus currentStatus = {0};

    if (ADL_OK != ADL_Overdrive6_CurrentStatus_Get (this->adapterId, &currentStatus))
    {
        printf("Failed to get custom performance levels info\n");
        return 0;
    }

    return (currentStatus.iMemoryClock/100);
}

int ADL_device::getMemClockStep()
{
    ADLOD6Capabilities od6Capabilities = {0};

    if (ADL_OK != ADL_Overdrive6_Capabilities_Get (this->adapterId, &od6Capabilities))
    {
        printf("Failed to get Overdrive capabilities\n");
        return 0;
    }

    return (od6Capabilities.sMemoryClockRange.iStep / 100);
}

int ADL_device::getMemClockMax()
{
    ADLOD6Capabilities od6Capabilities = {0};

    if (ADL_OK != ADL_Overdrive6_Capabilities_Get (this->adapterId, &od6Capabilities))
    {
        printf("Failed to get Overdrive capabilities\n");
        return 0;
    }

    return (od6Capabilities.sMemoryClockRange.iMax / 100);
}

int ADL_device::getMemClockMin()
{
    ADLOD6Capabilities od6Capabilities = {0};

    if (ADL_OK != ADL_Overdrive6_Capabilities_Get (this->adapterId, &od6Capabilities))
    {
        printf("Failed to get Overdrive capabilities\n");
        return 0;
    }

    return (od6Capabilities.sMemoryClockRange.iMin / 100);
}

int ADL_device::getHeat()
{
    int temperature;

    if (ADL_OK != ADL_Overdrive6_Temperature_Get (this->adapterId, &temperature))
    {
        printf("Failed to get GPU temperature\n");
        return 0;
    }

    return (temperature/1000);//            printf ("GPU temperature is %d degrees celsius \n", temperature / 1000); //The temperature is returned in mili-degree of Celsius
}

int ADL_device::getFan()
{
    ADLOD6FanSpeedInfo fanSpeedInfo = {0};

    if (ADL_OK != ADL_Overdrive6_FanSpeed_Get (this->adapterId, &fanSpeedInfo))
    {
        printf("Failed to get fan speed info\n");
        return 0;
    }

    return fanSpeedInfo.iFanSpeedRPM;
}

int ADL_device::getFanMax()
{
    ADLOD6ThermalControllerCaps fanSpeedInfo = {0};

    if (ADL_OK != ADL_Overdrive6_ThermalController_Caps (this->adapterId, &fanSpeedInfo))
    {
        printf("Failed to get fan speed info\n");
        return 0;
    }

    return fanSpeedInfo.iFanMaxRPM;
}

int ADL_device::getVoltage()
{
    int powerControlSupported;

    if (ADL_OK != ADL_Overdrive6_PowerControl_Caps (adapterId, &powerControlSupported))
    {
        printf("Failed to get power control capabilities\n");
        return 0;
    }

    //ADLOD6PowerControlInfo powerControlInfo;
    int powerControlCurrent;
    int powerControlDefault;

    if( powerControlSupported )
    {
        //if (ADL_OK != ADL_Overdrive6_PowerControlInfo_Get (adapterId, &powerControlInfo))
        //{
        //    printf("Failed to get power control information\n");
        //    return 0;
        //}

        if (ADL_OK != ADL_Overdrive6_PowerControl_Get (adapterId, &powerControlCurrent, &powerControlDefault))
        {
            printf("Failed to get power control current and default settings\n");
            return 0;
        }

        //printf ("Power Control range: %d...%d with step of %d \n", powerControlInfo.iMinValue, powerControlInfo.iMaxValue, powerControlInfo.iStepValue);
        //printf ("Power Control current level: %d \n", powerControlCurrent);
        //printf ("Power Control default level: %d \n", powerControlDefault);
        return powerControlCurrent;
    }

    return 0;
}

int ADL_device::getVoltageMax()
{
    int powerControlSupported;

    if (ADL_OK != ADL_Overdrive6_PowerControl_Caps (adapterId, &powerControlSupported))
    {
        printf("Failed to get power control capabilities\n");
        return 0;
    }

    ADLOD6PowerControlInfo powerControlInfo;
    //int powerControlCurrent;
    //int powerControlDefault;

    if( powerControlSupported )
    {
        if (ADL_OK != ADL_Overdrive6_PowerControlInfo_Get (adapterId, &powerControlInfo))
        {
            printf("Failed to get power control information\n");
            return 0;
        }

        //if (ADL_OK != ADL_Overdrive6_PowerControl_Get (adapterId, &powerControlCurrent, &powerControlDefault))
        //{
        //    printf("Failed to get power control current and default settings\n");
        //    return 0;
        //}

        //printf ("Power Control range: %d...%d with step of %d \n", powerControlInfo.iMinValue, powerControlInfo.iMaxValue, powerControlInfo.iStepValue);
        //printf ("Power Control current level: %d \n", powerControlCurrent);
        //printf ("Power Control default level: %d \n", powerControlDefault);
        return powerControlInfo.iMaxValue;
    }

    return 0;
}

int ADL_device::getVoltageStep()
{
    int powerControlSupported;

    if (ADL_OK != ADL_Overdrive6_PowerControl_Caps (adapterId, &powerControlSupported))
    {
        printf("Failed to get power control capabilities\n");
        return 0;
    }

    ADLOD6PowerControlInfo powerControlInfo;
    //int powerControlCurrent;
    //int powerControlDefault;

    if( powerControlSupported )
    {
        if (ADL_OK != ADL_Overdrive6_PowerControlInfo_Get (adapterId, &powerControlInfo))
        {
            printf("Failed to get power control information\n");
            return 0;
        }

        //if (ADL_OK != ADL_Overdrive6_PowerControl_Get (adapterId, &powerControlCurrent, &powerControlDefault))
        //{
        //    printf("Failed to get power control current and default settings\n");
        //    return 0;
        //}

        //printf ("Power Control range: %d...%d with step of %d \n", powerControlInfo.iMinValue, powerControlInfo.iMaxValue, powerControlInfo.iStepValue);
        //printf ("Power Control current level: %d \n", powerControlCurrent);
        //printf ("Power Control default level: %d \n", powerControlDefault);
        return powerControlInfo.iStepValue;
    }

    return 0;
}

void ADL_device::setGPUClock( int newClock )
{
    int size = sizeof (ADLOD6StateInfo) + sizeof (ADLOD6PerformanceLevel);
    ADLOD6StateInfo * customStateInfo = (ADLOD6StateInfo*) malloc (size);
    memset ((void*)customStateInfo, 0, size);
    customStateInfo->iNumberOfPerformanceLevels = 2;

    if (ADL_OK != ADL_Overdrive6_StateInfo_Get (adapterId, ADL_OD6_GETSTATEINFO_CUSTOM_PERFORMANCE, customStateInfo))
    {
        printf("Failed to get custom performance levels info\n");
        return;
    }

    if( this->getGPUClockMax() > newClock )
    {
        return;
    }

    customStateInfo->aLevels[1].iEngineClock = newClock * 100;

    if (ADL_OK != ADL_Overdrive6_State_Set (adapterId, ADL_OD6_SETSTATE_PERFORMANCE, customStateInfo)) // This is the only value that is currently accepted
    {
        printf ("Failed to set new effective core clock\n");
    }

    free( customStateInfo );
}

void ADL_device::setMemClock( int newClock )
{
    int size = sizeof (ADLOD6StateInfo) + sizeof (ADLOD6PerformanceLevel);
    ADLOD6StateInfo * customStateInfo = (ADLOD6StateInfo*) malloc (size);
    memset ((void*)customStateInfo, 0, size);
    customStateInfo->iNumberOfPerformanceLevels = 2;

    if (ADL_OK != ADL_Overdrive6_StateInfo_Get (adapterId, ADL_OD6_GETSTATEINFO_CUSTOM_PERFORMANCE, customStateInfo))
    {
        printf("Failed to get custom performance levels info\n");
        return;
    }

    if( this->getMemClockMax() > newClock )
    {
        return;
    }

    customStateInfo->aLevels[1].iMemoryClock = newClock * 100;

    if (ADL_OK != ADL_Overdrive6_State_Set (adapterId, ADL_OD6_SETSTATE_PERFORMANCE, customStateInfo)) // This is the only value that is currently accepted
    {
        printf ("Failed to set new effective core clock\n");
    }

    free( customStateInfo );
}

void ADL_device::upGPUClock()
{
    int size = sizeof (ADLOD6StateInfo) + sizeof (ADLOD6PerformanceLevel);
    ADLOD6StateInfo * customStateInfo = (ADLOD6StateInfo*) malloc (size);
    memset ((void*)customStateInfo, 0, size);
    customStateInfo->iNumberOfPerformanceLevels = 2;

    if (ADL_OK != ADL_Overdrive6_StateInfo_Get (adapterId, ADL_OD6_GETSTATEINFO_CUSTOM_PERFORMANCE, customStateInfo))
    {
        printf("Failed to get custom performance levels info\n");
        return;
    }

    customStateInfo->aLevels[1].iEngineClock += (this->getGPUClockStep()) * 100;

    if( (customStateInfo->aLevels[1].iEngineClock / 100) >= this->getGPUClockMax() )
    {
        free( customStateInfo );
        return;
    }

    if (ADL_OK != ADL_Overdrive6_State_Set (adapterId, ADL_OD6_SETSTATE_PERFORMANCE, customStateInfo)) // This is the only value that is currently accepted
    {
        printf ("Failed to set new effective core clock\n");
    }

    free( customStateInfo );
}

void ADL_device::downGPUClock()
{
    int size = sizeof (ADLOD6StateInfo) + sizeof (ADLOD6PerformanceLevel);
    ADLOD6StateInfo * customStateInfo = (ADLOD6StateInfo*) malloc (size);
    memset ((void*)customStateInfo, 0, size);
    customStateInfo->iNumberOfPerformanceLevels = 2;

    if (ADL_OK != ADL_Overdrive6_StateInfo_Get (adapterId, ADL_OD6_GETSTATEINFO_CUSTOM_PERFORMANCE, customStateInfo))
    {
        printf("Failed to get custom performance levels info\n");
        return;
    }

    customStateInfo->aLevels[1].iEngineClock -= (this->getGPUClockStep()) * 100;

    if( (customStateInfo->aLevels[1].iEngineClock / 100) < this->getGPUClockMin() )
    {
        free( customStateInfo );
        return;
    }

    if (ADL_OK != ADL_Overdrive6_State_Set (adapterId, ADL_OD6_SETSTATE_PERFORMANCE, customStateInfo)) // This is the only value that is currently accepted
    {
        printf ("Failed to set new effective core clock\n");
    }

    free( customStateInfo );
}

void ADL_device::upMemClock()
{
    int size = sizeof (ADLOD6StateInfo) + sizeof (ADLOD6PerformanceLevel);
    ADLOD6StateInfo * customStateInfo = (ADLOD6StateInfo*) malloc (size);
    memset ((void*)customStateInfo, 0, size);
    customStateInfo->iNumberOfPerformanceLevels = 2;

    if (ADL_OK != ADL_Overdrive6_StateInfo_Get (adapterId, ADL_OD6_GETSTATEINFO_CUSTOM_PERFORMANCE, customStateInfo))
    {
        printf("Failed to get custom performance levels info\n");
        return;
    }

    customStateInfo->aLevels[1].iMemoryClock += (this->getMemClockStep()) * 100;

    if( (customStateInfo->aLevels[1].iMemoryClock / 100) >= this->getMemClockMax() )
    {
        free( customStateInfo );
        return;
    }

    if (ADL_OK != ADL_Overdrive6_State_Set (adapterId, ADL_OD6_SETSTATE_PERFORMANCE, customStateInfo)) // This is the only value that is currently accepted
    {
        printf ("Failed to set new effective core clock\n");
    }

    free( customStateInfo );
}

void ADL_device::downMemClock()
{
    int size = sizeof (ADLOD6StateInfo) + sizeof (ADLOD6PerformanceLevel);
    ADLOD6StateInfo * customStateInfo = (ADLOD6StateInfo*) malloc (size);
    memset ((void*)customStateInfo, 0, size);
    customStateInfo->iNumberOfPerformanceLevels = 2;

    if (ADL_OK != ADL_Overdrive6_StateInfo_Get (adapterId, ADL_OD6_GETSTATEINFO_CUSTOM_PERFORMANCE, customStateInfo))
    {
        printf("Failed to get custom performance levels info\n");
        return;
    }

    customStateInfo->aLevels[1].iMemoryClock -= (this->getMemClockStep()) * 100;

    if( (customStateInfo->aLevels[1].iMemoryClock / 100) < this->getMemClockMin() )
    {
        free( customStateInfo );
        return;
    }

    if (ADL_OK != ADL_Overdrive6_State_Set (adapterId, ADL_OD6_SETSTATE_PERFORMANCE, customStateInfo)) // This is the only value that is currently accepted
    {
        printf ("Failed to set new effective core clock\n");
    }

    free( customStateInfo );
}

#include "hostwindow.h"
#include "ui_hostwindow.h"

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include <QObject>

#include "adl_define.h"
#include "adl/adl_sdk.h"

HostWindow::HostWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::HostWindow)
{
    ui->setupUi(this);

    QObject::connect( this->ui->device_list,
                        SIGNAL(clicked(QModelIndex)),
                      this,
                        SLOT(tryChangeDevice()) );

    this->ui->lcd_gpu->setPalette( Qt::green );
    this->ui->lcd_fun->setPalette( Qt::green );
    this->ui->lcd_memory->setPalette( Qt::green );
    this->ui->lcd_voltage->setPalette( Qt::green );

    this->ui->lcd_term->setPalette( Qt::red );


    {
        LPAdapterInfo lpAdapterInfo = NULL;

        ADL_ADAPTER_ACTIVE_GET ADL_Adapter_Active_Get;
        ADL_ADAPTER_ADAPTERINFO_GET	ADL_Adapter_AdapterInfo_Get;

        ADL_Adapter_Active_Get = (ADL_ADAPTER_ACTIVE_GET)GetProcAddress(ADL_device::gethDLL(), "ADL_Adapter_Active_Get");
        ADL_Adapter_AdapterInfo_Get = (ADL_ADAPTER_ADAPTERINFO_GET) GetProcAddress(ADL_device::gethDLL(),"ADL_Adapter_AdapterInfo_Get");

        int device_count = ADL_device::getDeviceCount();

        lpAdapterInfo = (LPAdapterInfo)malloc ( sizeof (AdapterInfo) * device_count );
        memset ( lpAdapterInfo,'\0', sizeof (AdapterInfo) * device_count );

                    // Get the AdapterInfo structure for all adapters in the system
        ADL_Adapter_AdapterInfo_Get (lpAdapterInfo, sizeof (AdapterInfo) * device_count);

              // Looking for first present and active adapter in the system
        int i;
        for ( i = 0; i < device_count; i++ )
        {
            int adapterActive = 0;
            AdapterInfo adapterInfo = lpAdapterInfo[ i ];
            ADL_Adapter_Active_Get(adapterInfo.iAdapterIndex , &adapterActive);

            if ( adapterActive
              && adapterInfo.iVendorID == AMDVENDORID ) //adapterActive &&
            {
                bool newAdapter = true;
                size_t deviceCount = this->device.count();
                for( int i = 0; i < deviceCount; i++ )
                {
                    if( this->device[i]->deviceId == adapterInfo.iDeviceNumber )
                    {
                        newAdapter = false;
                        break;
                    }
                }

                if( newAdapter )
                {
                    this->device.append( new ADL_device(adapterInfo) );
                    this->ui->device_list->addItem( (this->device.last())->adapterName );
                }

                //std::cout << adapterInfo.iAdapterIndex << " " << adapterInfo.strAdapterName << " " << adapterInfo.iDeviceNumber << std::endl;
                //adapterId = adapterInfo.iAdapterIndex;
                //break;
            }
        }

        free( lpAdapterInfo );
    }

    this->changeDevice( 0 );

    this->updateTimer = new QTimer(this);
    QObject::connect(this->updateTimer, SIGNAL(timeout()), this, SLOT(updateInfo()));
    this->updateTimer->start(1000);

    QObject::connect( this->ui->spinBox_gpu,
                        SIGNAL(valueChanged(int)),
                      this,
                        SLOT(trySetGPUClock(int)) );
}

void HostWindow::trySetGPUClock( int newClock )
{
    if( newClock > 1 )
    {
        this->device[this->currentDevice]->upGPUClock();
    }
    else if( newClock < 1 )
    {
        this->device[this->currentDevice]->downGPUClock();
    }

    this->ui->spinBox_gpu->setValue(1);
}

void HostWindow::trySetMemClock( int newClock )
{

}

HostWindow::~HostWindow()
{
    delete ui;
}

void HostWindow::changeDevice( int newDevice )
{
    this->currentDevice = newDevice;
    this->ui->info_box->setTitle( this->device[newDevice]->adapterName );
    updateInfo();
}

void HostWindow::tryChangeDevice()
{
    int newDevice = this->ui->device_list->currentRow();
    changeDevice( newDevice );
}

void HostWindow::updateInfo()
{
    this->ui->lcd_gpu->display( this->device[this->currentDevice]->getGPUClock() );

    QString max_text = "";
    max_text.append( QString::number( this->device[this->currentDevice]->getGPUClockMax() ) );
    max_text.append( " max\n" );
    max_text.append( QString::number( this->device[this->currentDevice]->getGPUClockMin() ) );
    max_text.append( " min" );
    this->ui->label_gpu_max_min->setText( max_text );

    this->ui->lcd_memory->display( this->device[this->currentDevice]->getMemClock() );

    max_text = "";
    max_text.append( QString::number( this->device[this->currentDevice]->getMemClockMax() ) );
    max_text.append( " max\n" );
    max_text.append( QString::number( this->device[this->currentDevice]->getMemClockMin() ) );
    max_text.append( " min" );
    this->ui->label_mem_max_min->setText( max_text );

    this->ui->lcd_fun->display( this->device[this->currentDevice]->getFan() );

    max_text = "";
    max_text.append( QString::number( this->device[this->currentDevice]->getFanMax() ) );
    max_text.append( " max\n" );
    //max_text.append( QString::number( this->device[this->currentDevice]->getMemClockMin() ) );
    //max_text.append( " min" );
    this->ui->label_fan_max_min->setText( max_text );

    this->ui->lcd_voltage->display( this->device[this->currentDevice]->getVoltage() );

    if( !(this->device[this->currentDevice]->getVoltage() ) )
    {
        this->ui->spinBox_voltage->setEnabled(false);
    }
    else
    {
        this->ui->spinBox_voltage->setEnabled(true);
    }

    this->ui->lcd_term->display( this->device[this->currentDevice]->getHeat() );
}

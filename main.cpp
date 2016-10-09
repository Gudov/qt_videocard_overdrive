#include "hostwindow.h"
#include <QApplication>

#include <iostream>
#include "adl_device.h"

int main(int argc, char *argv[])
{
    std::cout << ADL_device::getDeviceCount() << std::endl;
    QApplication a(argc, argv);
    HostWindow w;
    w.show();

    return a.exec();
}

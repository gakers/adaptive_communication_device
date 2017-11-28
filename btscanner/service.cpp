#include "service.h"

#include <qbluetoothaddress.h>
#include <qbluetoothservicediscoveryagent.h>
#include <qbluetoothserviceinfo.h>
#include <qbluetoothlocaldevice.h>
#include <qbluetoothuuid.h>


ServiceDiscoveryDialog::ServiceDiscoveryDialog(const QString &name,
                                               const QBluetoothAddress &address, QWidget *parent)
:   QDialog(parent), ui(new Ui_ServiceDiscovery)
{
    ui->setupUi(this);

    //Using default Bluetooth adapter
    QBluetoothLocalDevice localDevice;
    QBluetoothAddress adapterAddress = localDevice.address();

    /*
     * In case of multiple Bluetooth adapters it is possible to
     * set which adapter will be used by providing MAC Address.
     * Example code:
     *
     * QBluetoothAddress adapterAddress("XX:XX:XX:XX:XX:XX");
     * discoveryAgent = new QBluetoothServiceDiscoveryAgent(adapterAddress);
     */

    discoveryAgent = new QBluetoothServiceDiscoveryAgent(adapterAddress);

    discoveryAgent->setRemoteAddress(address);

    setWindowTitle(name);

    connect(discoveryAgent, SIGNAL(serviceDiscovered(QBluetoothServiceInfo)),
            this, SLOT(addService(QBluetoothServiceInfo)));
    connect(discoveryAgent, SIGNAL(finished()), ui->status, SLOT(hide()));

    discoveryAgent->start();
}

ServiceDiscoveryDialog::~ServiceDiscoveryDialog()
{
    delete discoveryAgent;
    delete ui;
}

void ServiceDiscoveryDialog::addService(const QBluetoothServiceInfo &info)
{
    if (info.serviceName().isEmpty())
        return;

    QString line = info.serviceName();
    if (!info.serviceDescription().isEmpty())
        line.append("\n\t" + info.serviceDescription());
    if (!info.serviceProvider().isEmpty())
        line.append("\n\t" + info.serviceProvider());

    ui->list->addItem(line);
}

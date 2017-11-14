#ifndef GPIOSCANNER_H
#define GPIOSCANNER_H
#include <QObject>
#include <QThread>
#include "gpio.h"
#include "mainwindow.h"

class gpioScanner : public QObject
{
    Q_OBJECT

public:
    explicit gpioScanner(QObject *parent = 0);
    
public slots:
    void doScanning();

signals:
    void button_pressed(const QString &button);
    
private:
    GPIO *gpio_up, *gpio_down, *gpio_left, *gpio_right, *gpio_select;
    
};

#endif // GPIOSCANNER_H

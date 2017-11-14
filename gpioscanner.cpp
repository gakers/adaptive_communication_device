#include "gpioscanner.h"

gpioScanner::gpioScanner(QObject *parent) : QObject( parent ) {
    gpio_up = new GPIO("17");
    gpio_up->export_gpio();
    QThread::msleep(50);
    gpio_up->setDir("in");

    gpio_down = new GPIO("27");
    gpio_down->export_gpio();
    QThread::msleep(50);
    gpio_down->setDir("in");

    gpio_left = new GPIO("22");
    gpio_left->export_gpio();
    QThread::msleep(50);
    gpio_left->setDir("in");

    gpio_right = new GPIO("10");
    gpio_right->export_gpio();
    QThread::msleep(50);
    gpio_right->setDir("in");

    gpio_select = new GPIO("9");
    gpio_select->export_gpio();
    QThread::msleep(50);
    gpio_select->setDir("in");
}

void gpioScanner::doScanning() {
    MainWindow* parent = qobject_cast<MainWindow*>(this->parent());
    int delay = 500;
    while(1) {
        delay = parent->get_delay();
        QThread::msleep(50);
        if(gpio_up->getVal() == "1") {
            QThread::msleep(delay);
            if(gpio_up->getVal() == "1")
                emit button_pressed("UP");
        }
        else if(gpio_down->getVal() == "1") {
            QThread::msleep(delay);
            if(gpio_down->getVal() == "1")
                emit button_pressed("DOWN");
        }
        else if(gpio_left->getVal() == "1") {
            QThread::msleep(delay);
            if(gpio_left->getVal() == "1")
                emit button_pressed("LEFT");
        }
        else if(gpio_right->getVal() == "1") {
            QThread::msleep(delay);
            if(gpio_right->getVal() == "1")
                emit button_pressed("RIGHT");
        }
        else if(gpio_select->getVal() == "1") {
            QThread::msleep(delay);
            if(gpio_select->getVal() == "1")
                emit button_pressed("SELECT");
        }
    }
}

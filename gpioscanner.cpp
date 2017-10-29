#include "gpioscanner.h"

gpioScanner::gpioScanner() : QObject() {
    gpio_up = new GPIO("17");
    gpio_up->export_gpio();
    gpio_up->setDir("in");

    gpio_down = new GPIO("27");
    gpio_down->export_gpio();
    gpio_down->setDir("in");

    gpio_left = new GPIO("22");
    gpio_left->export_gpio();
    gpio_left->setDir("in");

    gpio_right = new GPIO("10");
    gpio_right->export_gpio();
    gpio_right->setDir("in");

    gpio_select = new GPIO("9");
    gpio_select->export_gpio();
    gpio_select->setDir("in");
}

void gpioScanner::doScanning() {
    while(1) {
        QThread::msleep(50);
        if(gpio_up->getVal() == "1") {
            QThread::msleep(500);       //see if button is held for 0.5 seconds
            if(gpio_up->getVal() == "1")
                emit button_pressed("UP");
        }
        else if(gpio_down->getVal() == "1") {
            QThread::msleep(500);
            if(gpio_down->getVal() == "1")
                emit button_pressed("DOWN");
        }
        else if(gpio_left->getVal() == "1") {
            QThread::msleep(500);
            if(gpio_left->getVal() == "1")
                emit button_pressed("LEFT");
        }
        else if(gpio_right->getVal() == "1") {
            QThread::msleep(500);
            if(gpio_right->getVal() == "1")
                emit button_pressed("RIGHT");
        }
        else if(gpio_select->getVal() == "1") {
            QThread::msleep(500);
            if(gpio_select->getVal() == "1")
                emit button_pressed("SELECT");
        }
    }
}

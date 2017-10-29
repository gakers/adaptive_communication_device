#include "gpio.h"

GPIO::GPIO(QString x) {
    num = x;
}

void GPIO::export_gpio() {
    QFile f("/sys/class/gpio/export");
    f.open(QIODevice::WriteOnly);
    QTextStream out(&f);
    out << num;
    f.close();
}

void GPIO::unexport_gpio() {
    QFile f("/sys/class/gpio/unexport");
    f.open(QIODevice::WriteOnly);
    QTextStream out(&f);
    out << num;
    f.close();
}

void GPIO::setDir(QString dir) {
    QFile f("/sys/class/gpio/gpio" + num + "/direction");
    f.open(QIODevice::WriteOnly);
    QTextStream out(&f);
    out << dir;
    f.close();
}

void GPIO::setVal(QString val) {
    QFile f("/sys/class/gpio/gpio" + num + "/value");
    f.open(QIODevice::WriteOnly);
    QTextStream out(&f);
    out << val;
    f.close();
}

QString GPIO::getVal() {
    QFile f("/sys/class/gpio/gpio" + num + "/value");
    f.open(QIODevice::ReadOnly);
    QTextStream in(&f);
    QString tmp = in.readLine();
    f.close();
    if(tmp != "0")
        tmp = "1";
     return tmp;
}

QString GPIO::get_num() { return num; }





















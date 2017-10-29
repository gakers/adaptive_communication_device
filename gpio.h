#ifndef GPIO_H
#define GPIO_H
#include <QFile>
#include <QTextStream>
#include <QVector>
#include <QDebug>

class GPIO
{
public:
    GPIO(QString x);

    void export_gpio();       //Export GPIO%num%
    void unexport_gpio();     //Unexport GPIO%num%
    void setDir(QString dir); //Set direction
    void setVal(QString val); //Set output value
    QString getVal();         //Get input value
    QString get_num();        //Returns pin num
    
private:
    QString num;

};

#endif // GPIO_H

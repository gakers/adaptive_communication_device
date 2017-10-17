#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QProcess>
#include <QVector>
#include <QDebug>
#include <QFont>
#include <QFile>
#include <QTextStream>
//#include <QTextToSpeech>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    ~MainWindow();
    explicit MainWindow(QWidget *parent = 0);
    void text_to_speech(QStringList sentence);

    /* CommonBoard Functions */
    void setup_common();
    void destroy_common();

    /* Keyboard Functions */
    void setup_keyboard();
    void destroy_keyboard();
    void update_words(QString word);
    void clear_words();
    QStringList predict_text(QString str);

    /* Menu Functions */
    void setup_menu();
    void destroy_menu();

public slots:
    void itemPressed(int row, int column);
    void keyPressed(int row, int column);
    void menuPressed(int row, int column);

private:
    Ui::MainWindow *ui;
    QStringList msg;
    QString word;
    QVector< QVector< QString > > item_list;
    const QVector< QString > fixed_items{"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};

    QVector< QStringList > alphabet{{"A", "B", "C", "D", "E", "F", "G", "H", "I"},
                                   {"J", "K", "L", "M", "N", "O", "P", "Q", "R"},
                                   {"S", "T", "U", "V", "W", "X", "Y", "Z", "SPACE"},
                                   {"BACKSPACE", "NULL", "NULL","CLEAR", "NULL", "NULL", "SPEAK", "NULL", "NULL"},
                                   {"WORD1", "NULL", "NULL", "WORD2", "NULL", "NULL", "MENU", "NULL", "NULL"},
                                   {"WORD4", "NULL", "NULL", "WORD5", "NULL", "NULL", "PHRASES", "NULL", "NULL"}};
    QVector< QString > menu_list{"POWER OFF", "CHANGE WORDS", "NULL", "NULL", "RETURN"};
};

#endif // MAINWINDOW_H

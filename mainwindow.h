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
#include <QThread>
#include <QLayout>
#include "corpus.h"
#include "gpio.h"
#include "gpioscanner.h"

namespace Ui {
class MainWindow;
}
    struct position {
        int x;
        int y;
    };
    struct voice {
        QString name;
        int pitch;
        int speed;

    };

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread scannerThread;

public:
    ~MainWindow();
    explicit MainWindow(QWidget *parent = 0);

    void text_to_speech(QStringList sentence);
    QVector<QString> predict_text(QString str);
    //void unix_signal_handler(int unused);

    /* Accessors */
    int get_delay();

    /* CommonBoard Functions */
    void setup_common();
    void destroy_common();

    /* Keyboard Functions */
    void setup_keyboard();
    void destroy_keyboard();
    void update_words(QString word);
    void clear_words();

    /* Menu Functions */
    void setup_menu();
    void destroy_menu();
        /* change phrases menu */
        void change_phrases();
        void destroy_custom_phrases();
        void set_phrase();
        void update_phrases(QString phrase);
        void destroy_custom_keyboard();
        /* change input delay menu */
        void setup_delay();
        void destroy_delay();
        /* change voice settings menu */
        void setup_voice();
        void destroy_voice();


public slots:
    //void signal_handler();
    void scan_output(QString button);
    void itemPressed(int row, int column);
    void keyPressed(int row, int column);
    void menuPressed(int row, int column);
    void custom_itemPressed(int row, int column);
    void custom_keyPressed(int row, int column);
    void delayPressed(int row, int column);
    void voicePressed(int row, int column);

signals:
    void start_scanning();

private:
    Ui::MainWindow *ui;
    position curPosition;
    int delay, temp_delay;
    gpioScanner *scanner;
    CorpusNode head;
    QVector<long> score;
    QStringList msg, dict;
    QString word;
    int which_word;
    voice speechVoice;
    QVector< QVector< QString > > item_list;
    const QVector< QString > fixed_items{"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};

    QVector< QStringList > alphabet{{"A", "B", "C", "D", "E", "F", "G", "H", "I"},
                                    {"J", "K", "L", "M", "N", "O", "P", "Q", "R"},
                                    {"S", "T", "U", "V", "W", "X", "Y", "Z", "SPACE"},
                                    {"BACKSPACE", "NULL", "NULL","CLEAR", "NULL", "NULL", "SPEAK", "NULL", "NULL"},
                                    {"WORD1", "NULL", "NULL", "WORD2", "NULL", "NULL", "MENU", "NULL", "NULL"},
                                    {"WORD3", "NULL", "NULL", "WORD4", "NULL", "NULL", "PHRASES", "NULL", "NULL"}};
    QVector< QString > menu_list{"POWER OFF", "CHANGE WORDS", "CHANGE INPUT DELAY", "CHANGE VOICE", "EXIT", "RETURN"};
    QVector< QString > delay_list{"INCREASE DELAY", "DECREASE DELAY", "TEST", "ACCEPT", "CANCEL"};
    QVector< QString > voice_list{"CHANGE VOICE", "INCREASE SPEED", "DECREASE SPEED", "INCREASE PITCH", "DECREASE PITCH", "TEST", "ACCEPT", "CANCEL"};
    QVector< QString > voices{"en", "en-us", "en-us+f3", "en-sc", "en-n", "en-rp", "en-wm"};
    QVector< QStringList > customKeyboard{{"A", "B", "C", "D", "E", "F", "G", "H", "I"},
                                          {"J", "K", "L", "M", "N", "O", "P", "Q", "R"},
                                          {"S", "T", "U", "V", "W", "X", "Y", "Z", "SPACE"},
                                          {"BACKSPACE", "NULL", "NULL","CLEAR", "NULL", "NULL", "ACCEPT", "NULL", "NULL"},
                                          {"WORD1", "NULL", "NULL", "WORD2", "NULL", "NULL", "CANCEL", "NULL", "NULL"},
                                          {"WORD4", "NULL", "NULL", "WORD5", "NULL", "NULL", " ", "NULL", "NULL"}};
};

#endif // MAINWINDOW_H

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
#include <QTextToSpeech>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void update_words(QString word);
    void clear_words();
    QStringList predict_text(QString str);
    void text_to_speech(QStringList sentence);

public slots:
    void keyPressed(int row, int column);

private:
    Ui::MainWindow *ui;
    QStringList msg;
    QString word;
    QVector<QStringList> alphabet{{"A", "B", "C", "D", "E", "F", "G", "H", "I"},
                                  {"J", "K", "L", "M", "N", "O", "P", "Q", "R"},
                                  {"S", "T", "U", "V", "W", "X", "Y", "Z", "SPACE"},
                                  {"BACKSPACE", "NULL", "NULL","CLEAR", "NULL", "NULL", "SPEAK", "NULL", "NULL"},
                                  {"WORD1", "NULL", "NULL", "WORD2", "NULL", "NULL", "WORD3", "NULL", "NULL"},
                                  {"WORD4", "NULL", "NULL", "WORD5", "NULL", "NULL", "MENU", "NULL", "NULL"}};

};

#endif // MAINWINDOW_H

#include "mainwindow.h"
#include "ui_mainwindow.h"

// Destructor
MainWindow::~MainWindow() {
    delete ui;
}

// Constructor
MainWindow::MainWindow( QWidget *parent ) : QMainWindow( parent ), ui( new Ui::MainWindow ) {
    ui->setupUi(this);
    // Create rows and columns
    ui->keyboard->setRowCount(6);
    ui->keyboard->setColumnCount(9);

    // Formatting
    ui->keyboard->verticalHeader()->hide();
    ui->keyboard->horizontalHeader()->hide();
    ui->keyboard->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    ui->keyboard->verticalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    QFont keyFont;
        keyFont.setPointSize( 14 );
        keyFont.setFamily( "Arial" );
        keyFont.setBold( true );
    ui->curMsg->setFont( keyFont );
    // Set up keys
    for( int i=0; i < ui->keyboard->rowCount(); i++ ) {
        for( int j=0; j < ui->keyboard->columnCount(); j++ ) {
            QTableWidgetItem *keyCell = ui->keyboard->item( i, j );
            if(!keyCell) {
                keyCell = new QTableWidgetItem;
                keyCell->setTextAlignment(Qt::AlignCenter);
                keyCell->setFont(keyFont);
                keyCell->setFlags( keyCell->flags() & ~Qt::ItemIsEditable );
                ui->keyboard->setItem( i, j, keyCell );
            }
            keyCell->setText( alphabet[i][j] );
            if( (keyCell->text() == "SPEAK") || (keyCell->text() == "CLEAR") || (keyCell->text() == "BACKSPACE") || (keyCell->text().contains("WORD")) || (keyCell->text() == "MENU") ) {
                ui->keyboard->setSpan( i, j, 1, 3 );
            }
            if( keyCell->text() == "SPEAK" )
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(90, 200, 90));
            else if( keyCell->text() == "MENU" )
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(140, 140, 255));
            else if( keyCell->text() == "CLEAR")
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(233, 90, 90));
            else if( keyCell->text() == "BACKSPACE")
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(240, 255, 90));
            else if( keyCell->text().contains("WORD")) {
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(225, 225, 225));
                //ui->keyboard->item(i, j)->setFlags( ui->keyboard->item(i, j)->flags() & ~Qt::ItemIsSelectable);
            }
        }
    }

    connect( ui->keyboard, SIGNAL( cellClicked( int,int ) ), SLOT( keyPressed( int,int ) ) );
    clear_words();
}

// Update predictive text words
void MainWindow::update_words(QString str) {
    clear_words();
    if(str.size() > 0) {
        QStringList words = predict_text(str);
        int count = words.size();
        if(count >= 1) {
            ui->keyboard->item(4, 0)->setText(words.at(0).toUpper());
            ui->keyboard->item(4, 0)->setBackgroundColor(QColor(255, 255, 255));
        }
        if(count >= 2) {
            ui->keyboard->item(4, 3)->setText(words.at(1).toUpper());
            ui->keyboard->item(4, 3)->setBackgroundColor(QColor(255, 255, 255));
        }
        if(count >= 3) {
            ui->keyboard->item(4, 6)->setText(words.at(2).toUpper());
            ui->keyboard->item(4, 6)->setBackgroundColor(QColor(255, 255, 255));
        }
        if(count >= 4) {
            ui->keyboard->item(5, 0)->setText(words.at(3).toUpper());
            ui->keyboard->item(5, 0)->setBackgroundColor(QColor(255, 255, 255));
        }
        if(count >= 5) {
            ui->keyboard->item(5, 3)->setText(words.at(4).toUpper());
            ui->keyboard->item(5, 3)->setBackgroundColor(QColor(255, 255, 255));
        }
    }
}

void MainWindow::clear_words() {
    ui->keyboard->item(4, 0)->setText("");
    ui->keyboard->item(4, 3)->setText("");
    ui->keyboard->item(4, 6)->setText("");
    ui->keyboard->item(5, 0)->setText("");
    ui->keyboard->item(5, 3)->setText("");

    ui->keyboard->item(4, 0)->setBackgroundColor(QColor(225, 225, 225));
    ui->keyboard->item(4, 3)->setBackgroundColor(QColor(225, 225, 225));
    ui->keyboard->item(4, 6)->setBackgroundColor(QColor(225, 225, 225));
    ui->keyboard->item(5, 0)->setBackgroundColor(QColor(225, 225, 225));
    ui->keyboard->item(5, 3)->setBackgroundColor(QColor(225, 225, 225));
}
QStringList MainWindow::predict_text(QString str) {
    QStringList dict, ret;
    QString temp;
    int count;

    QFile f("/home/gakers/txt2speech/dict.txt");
    f.open(QIODevice::ReadOnly);
    QTextStream in(&f);
    while( !in.atEnd() ) {
        temp = in.readLine();
        dict << temp;
    }

    temp.clear();
    temp = str;
    for( int i=0; i<dict.size(); i++ ) {
        if( dict.at(i).startsWith(temp, Qt::CaseInsensitive) ) {
            ret << dict.at(i);
            count++;
            if(count == 5) break;
        }
    }
    return ret;
}
void MainWindow::text_to_speech(QStringList sentence) {
    QTextToSpeech speech;
    QStringList tmp = sentence;
    speech.say(tmp.join(" "));
    //spd-say tmp.join;
}

//SLOTS
void MainWindow::keyPressed( int row, int column ) {
    QString cmd = alphabet[row][column];

    if(cmd == "SPACE") {
        if( word.size() > 0 ) {
            msg << word ;
            word.clear();
            clear_words();
        }
    }
    else if( cmd == "BACKSPACE" ) {
        if( word.size() > 0 ) {
            word.remove( ( word.size() - 1 ), 1 );
            update_words( word );
        }
        // Handle backspace-ing to previous word
        else if ( msg.size() > 0 ) {
            word = msg.takeLast();
            update_words( word );
        }
    }
    else if ( cmd == "CLEAR" ) {
        word.clear();
        msg.clear();
        clear_words();
    }
    else if ( cmd == "SPEAK" ) {
        msg << word;
        text_to_speech(msg);
        word.clear();
        msg.clear();
        clear_words();
    }
    else if ( cmd.contains( "WORD" ) ) {
        QString tmp = ui->keyboard->item(row, column)->text();
        if(tmp.size() > 0) {
            word.clear();
            msg << tmp;
            clear_words();
        }
    }
    else if( cmd == "NULL" ) { } // Do nothing, shouldn't ever happen anyway
    else {                       // Just add a character to current word
        word.append( cmd );
        update_words( word );
    }
    QString tmp = msg.join( " " );
    tmp.append( " " );
    tmp.append( word );
    ui->curMsg->setText( tmp );
}

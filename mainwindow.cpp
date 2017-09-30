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
            if( (keyCell->text() == "DONE") || (keyCell->text() == "CLEAR") || (keyCell->text() == "BACKSPACE") || (keyCell->text().contains("WORD")) || (keyCell->text() == "MENU") )
                ui->keyboard->setSpan( i, j, 1, 3 );
        }
    }

    connect( ui->keyboard, SIGNAL( cellClicked( int,int ) ), SLOT( keyPressed( int,int ) ) );
    clear_words();
}

// Update predictive text words
void MainWindow::predict_text(QString word) {
    QStringList words;
    /*words = generate_predictive_text(word);*/
    /* fake predicted words */
    words << word << word << word << word << word << word;
    /************************/
    ui->keyboard->item(4, 0)->setText(words.at(0));
    ui->keyboard->item(4, 3)->setText(words.at(1));
    ui->keyboard->item(4, 6)->setText(words.at(2));
    ui->keyboard->item(5, 0)->setText(words.at(3));
    ui->keyboard->item(5, 3)->setText(words.at(4));
    //ui->keyboard->item(5, 6)->setText(words.at(5));
}

void MainWindow::clear_words() {
    ui->keyboard->item(4, 0)->setText("");
    ui->keyboard->item(4, 3)->setText("");
    ui->keyboard->item(4, 6)->setText("");
    ui->keyboard->item(5, 0)->setText("");
    ui->keyboard->item(5, 3)->setText("");
    //ui->keyboard->item(5, 6)->setText("");
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
            predict_text( word );
        }
        // Handle backspace-ing to previous word
        else if ( msg.size() > 0 ) {
            word = msg.takeLast();
            predict_text( word );
        }
    }
    else if ( cmd == "CLEAR" ) {
        word.clear();
        msg.clear();
        clear_words();
    }
    else if ( cmd == "DONE" ) {
        msg << word;
        //text_to_speech(msg);
        word.clear();
        msg.clear();
        clear_words();
    }
    else if ( cmd.contains( "WORD" ) ) {
        word.clear();
        msg << ui->keyboard->item(row, column)->text();
        clear_words();
    }
    else if( cmd == "NULL" ) { } // Do nothing, shouldn't ever happen anyway
    else {                     // Just add a character to current word
        word.append( cmd );
        predict_text( word );
    }
    QString tmp = msg.join( " " );
    tmp.append( " " );
    tmp.append( word );
    ui->curMsg->setText( tmp );
}

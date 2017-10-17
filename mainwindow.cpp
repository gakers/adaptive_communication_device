#include "mainwindow.h"
#include "ui_mainwindow.h"

// Destructor
MainWindow::~MainWindow() {
    delete ui;
}

// Constructor
MainWindow::MainWindow( QWidget *parent ) : QMainWindow( parent ), ui( new Ui::MainWindow ) {
    ui->setupUi(this);
    setup_common();

}

void MainWindow::text_to_speech(QStringList sentence) {
    //QTextToSpeech speech;
    //QStringList tmp = sentence;
    //speech.say(tmp.join(" "));
    //spd-say tmp.join;
}

/*************************************************************************************************************************
 *  COMMON WORDS/PHRASES BOARD
 *
 *  setup_common()
 *
 *
 *
 */
void MainWindow::setup_common() {
    ui->curMsg->hide();
    ui->keyboard->setRowCount(6);
    ui->keyboard->setColumnCount(5);

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

    /* read phrases list */
    item_list.resize(6);
    QFile f("/home/gakers/txt2speech/phrase_list.txt");
    f.open(QIODevice::ReadOnly);
    QTextStream in(&f);
    for(int i=0; i<4; i++) { /* read saved phrases into board contents list */
        item_list[i].resize(5);
        for(int j=0; j<5; j++) {
            item_list[i][j] = in.readLine().toUpper();
        }
    }
    f.close();
    int k = 0;
    for(int i=4; i<6; i++) { /* read fixed items into board contents list */
        item_list[i].resize(5);
        for(int j=0; j<5; j++) {
            item_list[i][j] = fixed_items[k];
            k++;
        }
    }
    for( int i=0; i < ui->keyboard->rowCount(); i++ ) {
        for( int j=0; j < ui->keyboard->columnCount(); j++ ) {
            QTableWidgetItem *itemCell = ui->keyboard->item( i, j );
            if(!itemCell) {
                itemCell = new QTableWidgetItem;
                itemCell->setTextAlignment(Qt::AlignCenter);
                itemCell->setFont(keyFont);
                itemCell->setFlags( itemCell->flags() & ~Qt::ItemIsEditable );
                ui->keyboard->setItem( i, j, itemCell );
            }
            itemCell->setText( item_list[i][j] );

            if( itemCell->text() == "KEYBOARD" ) {
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(140, 140, 255));
            }
        }
    }
    connect( ui->keyboard, SIGNAL( cellClicked( int,int ) ), this, SLOT( itemPressed( int,int ) ) );
}

void MainWindow::destroy_common() {
    disconnect( ui->keyboard, SIGNAL( cellClicked( int,int ) ), this, SLOT( itemPressed( int,int ) ) );
    ui->keyboard->clearContents();
    ui->curMsg->show();
}

void MainWindow::itemPressed(int row, int column) {
    QString cmd = item_list[row][column];
    if(cmd == "KEYBOARD") { /* switch to keyboard */
        destroy_common();
        setup_keyboard();
    }
    else { /* say the phrase */
        QStringList sentence = cmd.split(" ");
        text_to_speech(sentence);
    }
}

/*************************************************************************************************************************
 *    KEYBOARD
 *
 *  setup_keyboard()
 *  destroy_keyboard()
 *  update_words()
 *  clear_words()
 *  predict_text()
 *  key_pressed()
 */

void MainWindow::setup_keyboard() {
    ui->curMsg->show();
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
            if( (keyCell->text() == "SPEAK") || (keyCell->text() == "CLEAR") || (keyCell->text() == "BACKSPACE") || (keyCell->text().contains("WORD")) || (keyCell->text() == "PHRASES") || (keyCell->text() == "MENU") ) {
                ui->keyboard->setSpan( i, j, 1, 3 );
            }
            if( keyCell->text() == "SPEAK" )
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(90, 200, 90));
            else if( keyCell->text() == "PHRASES" )
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(140, 140, 255));
            else if( keyCell->text() == "CLEAR")
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(233, 90, 90));
            else if( keyCell->text() == "BACKSPACE")
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(240, 255, 90));
            else if( keyCell->text() == "MENU")
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(233, 90, 200));
            else if( keyCell->text().contains("WORD")) {
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(225, 225, 225));
            }
        }
    }

    connect( ui->keyboard, SIGNAL( cellClicked( int,int ) ), this, SLOT( keyPressed( int,int ) ) );
    clear_words();
}

void MainWindow::destroy_keyboard() {
    /* set spanning to default */
    for( int i=0; i < ui->keyboard->rowCount(); i++ ) {
        for( int j=0; j < ui->keyboard->columnCount(); j++ ) {
            ui->keyboard->setSpan( i, j, 1, 1 );
        }
    }
    disconnect( ui->keyboard, SIGNAL( cellClicked( int,int ) ), this, SLOT( keyPressed( int,int ) ) );
    ui->keyboard->clearContents();
    ui->curMsg->hide();
    word.clear();
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
            ui->keyboard->item(5, 0)->setText(words.at(3).toUpper());
            ui->keyboard->item(5, 0)->setBackgroundColor(QColor(255, 255, 255));
        }
        if(count >= 4) {
            ui->keyboard->item(5, 3)->setText(words.at(4).toUpper());
            ui->keyboard->item(5, 3)->setBackgroundColor(QColor(255, 255, 255));
        }

        /*
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
        }*/
    }
}

void MainWindow::clear_words() {
    ui->keyboard->item(4, 0)->setText("");
    ui->keyboard->item(4, 3)->setText("");
    //ui->keyboard->item(4, 6)->setText("");
    ui->keyboard->item(5, 0)->setText("");
    ui->keyboard->item(5, 3)->setText("");

    ui->keyboard->item(4, 0)->setBackgroundColor(QColor(225, 225, 225));
    ui->keyboard->item(4, 3)->setBackgroundColor(QColor(225, 225, 225));
    //ui->keyboard->item(4, 6)->setBackgroundColor(QColor(225, 225, 225));
    ui->keyboard->item(5, 0)->setBackgroundColor(QColor(225, 225, 225));
    ui->keyboard->item(5, 3)->setBackgroundColor(QColor(225, 225, 225));
}
QStringList MainWindow::predict_text(QString str) {
    QStringList dict, ret, line;
    QVector<int> score;

    QFile f("/home/gakers/txt2speech/google-books-common-words.txt");
    f.open(QIODevice::ReadOnly);
    QTextStream in(&f);
    while( !in.atEnd() ) {
        line = in.readLine().split("\t");
        dict << line.at(0);
        score <<line.at(1).toInt();
    }
    f.close();

    /* indices of 5 highest scoring words */
    QVector<int> w(5);
    w[0] = dict.size() - 1;
    w[1] = dict.size() - 1;
    w[2] = dict.size() - 1;
    w[3] = dict.size() - 1;
    w[4] = dict.size() - 1;

    for( int i=0; i<dict.size(); i++ ) { /* add top 5 words to list */
        if( dict.at(i).startsWith(str, Qt::CaseInsensitive) ) {
            if( score[i] > score[w[0]] ) {       /* new highest scoring word */
                w[4] = w[3]; w[3] = w[2]; w[2] = w[1]; w[1] = w[0]; w[0] = i;
            }
            else if( score [i] > score[w[1]] ) { /* new second-highest scoring word */
                w[4] = w[3]; w[3] = w[2]; w[2] = w[1]; w[1] = i;
            }
            else if( score[i] > score[w[2]] ) {  /* new third-highest scoring word */
                w[4] = w[3]; w[3] = w[2]; w[2] = i;
            }
            else if( score[i] > score[w[3]] ) {
                w[4] = w[3]; w[3] = i;
            }
            else if( score[i] > score[w[4]] ) {
                w[4] = i;
            }
        }
    }
    for( int i=0; i<5; i++ ) {
        if( w[i] != (dict.size() - 1) ) {
            ret << dict.at(w[i]);
        }
    }
    return ret;
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
    else if ( cmd == "MENU"  ) {
        destroy_keyboard();
        setup_menu();
    }
    else if ( cmd == "PHRASES"  ) {
        destroy_keyboard();
        setup_common();
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


/*************************************************************************************************************************
 *  MENU
 *
 *  setup_menu()
 *  destory_menu()
 *
 *
 */
void MainWindow::setup_menu() {
    ui->curMsg->hide();
    ui->keyboard->setRowCount(5);
    ui->keyboard->setColumnCount(1);

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


    for( int i=0; i < ui->keyboard->rowCount(); i++ ) {
            QTableWidgetItem *itemCell = ui->keyboard->item( i, 0 );
            if(!itemCell) {
                itemCell = new QTableWidgetItem;
                itemCell->setTextAlignment(Qt::AlignCenter);
                itemCell->setFont(keyFont);
                itemCell->setFlags( itemCell->flags() & ~Qt::ItemIsEditable );
                ui->keyboard->setItem( i, 0, itemCell );
            }
            itemCell->setText( menu_list[i] );
            ui->keyboard->item(i, 0)->setBackgroundColor(QColor(230, 230, 255));
        }
    connect( ui->keyboard, SIGNAL( cellClicked( int,int ) ), this, SLOT( menuPressed( int,int ) ) );
}

void MainWindow::destroy_menu() {
    disconnect( ui->keyboard, SIGNAL( cellClicked( int,int ) ), this, SLOT( menuPressed( int,int ) ) );
    ui->keyboard->clearContents();
    ui->curMsg->show();
}

void MainWindow::menuPressed(int row, int column) {
    QString cmd = menu_list[row];
    if(cmd == "POWER OFF") { /* power off device */
        //system("shutdown now");
    }
    else if(cmd == "RETURN") { /* exit the menu */
        destroy_menu();
        setup_keyboard();
    }
}

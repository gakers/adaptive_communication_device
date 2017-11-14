#include "mainwindow.h"
#include "ui_mainwindow.h"

// Destructor
MainWindow::~MainWindow() {
    delete ui;
}

// Constructor
MainWindow::MainWindow( QWidget *parent ) : QMainWindow( parent ), ui( new Ui::MainWindow ) {
    ui->setupUi(this);
    QFont titleFont;
    titleFont.setPointSize( 18 );
    titleFont.setFamily( "Arial" );
    titleFont.setBold( true );
    ui->title->setFont(titleFont);
    setup_common();
    QVBoxLayout *vbl = new QVBoxLayout(ui->centralWidget);
    vbl->setStretch(1, 0);
    ui->centralWidget->setLayout(vbl);
    QHBoxLayout *hbl = new QHBoxLayout;
    hbl->setStretch(1, 0);
    ui->keyboard->setLayout(hbl);
    ui->menuBar->hide();
    ui->mainToolBar->hide();
    /* Create dictionary from file */
    build_corpus(":/google-books-common-words.txt", head);

    /* Get input delay from file */
    QFile f("/home/gakers/txt2speech/delay.settings");
    f.open(QIODevice::ReadOnly);
    QTextStream in(&f);
    delay = in.readLine().toInt();
    f.close();

    /* get voice settings from file */
    QFile f1("/home/gakers/txt2speech/voice.settings");
    f1.open(QIODevice::ReadOnly);
    QTextStream in1(&f1);
    QString set, val;
    QStringList tmp;
    while(!in1.atEnd()) {
        tmp = in1.readLine().split(" ");
        if(tmp.at(0) == "NAME")
            speechVoice.name = tmp.at(1);
        else if(tmp.at(0) == "PITCH")
            speechVoice.pitch = tmp.at(1).toInt();
        else if(tmp.at(0) == "SPEED")
            speechVoice.speed = tmp.at(1).toInt();
    }
    f1.close();
    /* Start GPIO scanning thread */
    gpioScanner *scanner = new gpioScanner;
    scanner->moveToThread(&scannerThread);
    connect(&scannerThread, &QThread::finished, scanner, &QObject::deleteLater);
    connect(this, SIGNAL(start_scanning()), scanner, SLOT(doScanning()));
    connect(scanner, SIGNAL(button_pressed(QString)), this, SLOT(scan_output(QString)));
    scannerThread.start();
    curPosition.x = 0;
    curPosition.y = 0;
    ui->keyboard->setCurrentCell(curPosition.y, curPosition.x);
    //emit start_scanning();
}

int MainWindow::get_delay() { return delay; }

void MainWindow::text_to_speech(QStringList sentence) {
    QProcess p;
    QStringList args;
    args << "-v" + speechVoice.name\
         << "-p" + QString::number(speechVoice.pitch)\
         << "-s" + QString::number(speechVoice.speed)\
         << sentence.join(" ");
    p.start("/usr/bin/espeak", args);
    p.waitForStarted();
    p.waitForFinished();
    p.close();
}

QVector<QString> MainWindow::predict_text(QString str) {
    QVector<QString> ret;
    head.get_words_starting_with(str, 5, ret);
    return ret;
}

void MainWindow::scan_output(QString button) {
    qDebug() << button;
    if(button == "SELECT") {
        ui->keyboard->cellActivated(curPosition.y, curPosition.x);
    } else {
        if(button == "UP")
            curPosition.y--;
        else if(button == "DOWN")
            curPosition.y++;
        else if(button == "RIGHT")
            curPosition.x++;
        else if(button == "LEFT")
            curPosition.x--;
    }
    //Update cursor position, implement wrapping.
    int max_x = ui->keyboard->columnCount() - 1;
    int max_y = ui->keyboard->rowCount() - 1;
    if(curPosition.y > max_y) //Wrap vertically
        curPosition.y = 0;
    else if(curPosition.y < 0)
        curPosition.y = max_y;
    if(curPosition.x > max_x) //Wrap horizontally
        curPosition.x = 0;
    else if(curPosition.x < 0)
        curPosition.x = max_x;

    //deal with wide cells
    while(ui->keyboard->item(curPosition.y, curPosition.x)->text() == "NULL") {
        if(button == "RIGHT") {
            curPosition.x++;
            if(curPosition.x > max_x)
                curPosition.x = 0;
        } else
            curPosition.x--;
    }
    ui->keyboard->setCurrentCell(curPosition.y, curPosition.x);
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
    curPosition.x = 0;
    curPosition.y = 0;
    ui->title->setText("Common Phrases");
    ui->curMsg->hide();
    ui->keyboard->setRowCount(6);
    ui->keyboard->setColumnCount(5);

    // Formatting
    ui->keyboard->verticalHeader()->hide();
    ui->keyboard->horizontalHeader()->hide();
    ui->keyboard->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    ui->keyboard->verticalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    QFont keyFont;
        keyFont.setPointSize( 18 );
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
   // connect( ui->keyboard, SIGNAL( cellClicked( int,int ) ), this, SLOT( itemPressed( int,int ) ) );
    connect( ui->keyboard, SIGNAL( cellActivated( int,int ) ), this, SLOT( itemPressed( int,int ) ) );

}

void MainWindow::destroy_common() {
    disconnect( ui->keyboard, SIGNAL( cellActivated( int,int ) ), this, SLOT( itemPressed( int,int ) ) );
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
    curPosition.x = 0;
    curPosition.y = 0;
    ui->title->setText("Keyboard");
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
        keyFont.setPointSize( 18 );
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
    connect( ui->keyboard, SIGNAL( cellActivated( int,int ) ), this, SLOT( keyPressed( int,int ) ) );
    clear_words();
    ui->curMsg->setText("");
}

void MainWindow::destroy_keyboard() {
    /* set spanning to default */
    for( int i=0; i < ui->keyboard->rowCount(); i++ ) {
        for( int j=0; j < ui->keyboard->columnCount(); j++ ) {
            ui->keyboard->setSpan( i, j, 1, 1 );
        }
    }
    disconnect( ui->keyboard, SIGNAL( cellActivated( int,int ) ), this, SLOT( keyPressed( int,int ) ) );
    ui->keyboard->clearContents();
    ui->curMsg->hide();
    word.clear();
}

// Update predictive text words
void MainWindow::update_words(QString str) {
    clear_words();
    if(str.size() > 0) {
        QVector<QString> words = predict_text(str);
        int count = words.size();
        if(count >= 1) {
            ui->keyboard->item(4, 0)->setText(words[0].toUpper());
            ui->keyboard->item(4, 0)->setBackgroundColor(QColor(255, 255, 255));
        }
        if(count >= 2) {
            ui->keyboard->item(4, 3)->setText(words[1].toUpper());
            ui->keyboard->item(4, 3)->setBackgroundColor(QColor(255, 255, 255));
        }
        if(count >= 3) {
            ui->keyboard->item(5, 0)->setText(words[2].toUpper());
            ui->keyboard->item(5, 0)->setBackgroundColor(QColor(255, 255, 255));
        }
        if(count >= 4) {
            ui->keyboard->item(5, 3)->setText(words[3].toUpper());
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
    curPosition.x = 0;
    curPosition.y = 0;
    ui->title->setText("Menu");
    ui->curMsg->hide();
    ui->keyboard->setRowCount(5);
    ui->keyboard->setColumnCount(1);

    // Formatting
    ui->keyboard->verticalHeader()->hide();
    ui->keyboard->horizontalHeader()->hide();
    ui->keyboard->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    ui->keyboard->verticalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    QFont keyFont;
        keyFont.setPointSize( 18 );
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
    connect( ui->keyboard, SIGNAL( cellActivated( int,int ) ), this, SLOT( menuPressed( int,int ) ) );
}

void MainWindow::destroy_menu() {
    disconnect( ui->keyboard, SIGNAL( cellActivated( int,int ) ), this, SLOT( menuPressed( int,int ) ) );
    ui->keyboard->clearContents();
    ui->curMsg->show();
}

void MainWindow::change_phrases() {
    curPosition.x = 0;
    curPosition.y = 0;
    /* set up phrase window */
    destroy_menu();
    ui->title->setText("Select Phrase to Change");
    ui->curMsg->hide();
    ui->keyboard->setRowCount(6);
    ui->keyboard->setColumnCount(5);

    // Formatting
    ui->keyboard->verticalHeader()->hide();
    ui->keyboard->horizontalHeader()->hide();
    ui->keyboard->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    ui->keyboard->verticalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    QFont keyFont;
        keyFont.setPointSize( 18 );
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
    for(int i=4; i<6; i++) {
        item_list[i].resize(5);
        for(int j=0; j<5; j++) {
            item_list[i][j] = "";
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

            if( itemCell->text() == "KEYBOARD" )
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(233, 200, 80));
            else if ( itemCell->text() == "")
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(230, 230, 230));
        }
    }
    ui->keyboard->item(3, 4)->setText("CANCEL"); /* add cancel button */
    connect( ui->keyboard, SIGNAL( cellActivated( int,int ) ), this, SLOT( custom_itemPressed( int,int ) ) );
}

void MainWindow::destroy_custom_phrases() {
    disconnect( ui->keyboard, SIGNAL( cellActivated( int,int ) ), this, SLOT( custom_itemPressed( int,int ) ) );
    ui->keyboard->clearContents();
    ui->curMsg->show();
}

void MainWindow::set_phrase() {
    curPosition.x = 0;
    curPosition.y = 0;
    /* set up keyboard */
    destroy_custom_phrases();
    ui->title->setText("Enter new phrase");
    ui->curMsg->show();
    /* Create rows and columns */
    ui->keyboard->setRowCount(6);
    ui->keyboard->setColumnCount(9);
    /* Formatting */
    ui->keyboard->verticalHeader()->hide();
    ui->keyboard->horizontalHeader()->hide();
    ui->keyboard->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    ui->keyboard->verticalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    QFont keyFont;
        keyFont.setPointSize( 18 );
        keyFont.setFamily( "Arial" );
        keyFont.setBold( true );
    ui->curMsg->setFont( keyFont );

    /* Set up keys */
    for(int i=0; i < ui->keyboard->rowCount(); i++) {
        for(int j=0; j < ui->keyboard->columnCount(); j++) {
            QTableWidgetItem *keyCell = ui->keyboard->item(i, j);
            if(!keyCell) {
                keyCell = new QTableWidgetItem;
                keyCell->setTextAlignment(Qt::AlignCenter);
                keyCell->setFont(keyFont);
                keyCell->setFlags( keyCell->flags() & ~Qt::ItemIsEditable );
                ui->keyboard->setItem(i, j, keyCell);
            }
            keyCell->setText(customKeyboard[i][j]);
            if((keyCell->text() == "ACCEPT") || (keyCell->text() == "CLEAR") || (keyCell->text() == "BACKSPACE") || (keyCell->text().contains("WORD")) || (keyCell->text() == "CANCEL") || (keyCell->text() == " ")) {
                ui->keyboard->setSpan(i, j, 1, 3);
            }
            if( keyCell->text() == "ACCEPT" )
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(90, 200, 90));
            else if( keyCell->text() == "CLEAR")
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(233, 90, 90));
            else if( keyCell->text() == "BACKSPACE")
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(240, 255, 90));
            else if( keyCell->text() == "CANCEL")
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(233, 200, 50));
            else if( (keyCell->text().contains("WORD")) || (keyCell->text() == " "))
                ui->keyboard->item(i, j)->setBackgroundColor(QColor(225, 225, 225));
        }
    }
    connect(ui->keyboard, SIGNAL(cellActivated(int, int)), this, SLOT(custom_keyPressed(int, int)));
    clear_words();
}

void MainWindow::destroy_custom_keyboard() {
    /* set spanning to default */
    for( int i=0; i < ui->keyboard->rowCount(); i++ ) {
        for( int j=0; j < ui->keyboard->columnCount(); j++ ) {
            ui->keyboard->setSpan( i, j, 1, 1 );
        }
    }
    disconnect(ui->keyboard, SIGNAL(cellActivated(int, int)), this, SLOT(custom_keyPressed(int, int)));
    ui->keyboard->clearContents();
    ui->curMsg->hide();
    word.clear();
}

void MainWindow::update_phrases(QString phrase) {
    /* read current phrases list */
    QVector<QString> tmplist(20);
    QFile f("/home/gakers/txt2speech/phrase_list.txt");
    f.open(QIODevice::ReadOnly);
    QTextStream in(&f);
    for(int i=0; i<20; i++) { /* read saved phrases into list */
        tmplist[i] = in.readLine().toUpper();
    }
    f.close();

    /* replace target phrase with new phrase */
    f.open(QIODevice::WriteOnly);
    QTextStream out(&f);
    tmplist[which_word] = phrase;
    for(int i=0; i<20; i++) { /* write new list to file */
        out << tmplist[i] <<"\n";
    }
    f.close();
}

/*** Change input delay ***/
void MainWindow::setup_delay() {
    curPosition.x = 0;
    curPosition.y = 0;
    ui->title->setText("Set Input Delay");
    ui->curMsg->show();
    ui->keyboard->setRowCount(5);
    ui->keyboard->setColumnCount(1);

    // Formatting
    ui->keyboard->verticalHeader()->hide();
    ui->keyboard->horizontalHeader()->hide();
    ui->keyboard->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    ui->keyboard->verticalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    QFont keyFont;
        keyFont.setPointSize( 18 );
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
            ui->keyboard->item(i, 0)->setBackgroundColor(QColor(230, 230, 255));
            itemCell->setText(delay_list[i]);
        }
    ui->curMsg->setText("CURRENT DELAY: " + QString::number(delay) + "ms");
    connect( ui->keyboard, SIGNAL( cellActivated( int,int ) ), this, SLOT( delayPressed( int,int ) ) );
}
void MainWindow::destroy_delay() {
    disconnect( ui->keyboard, SIGNAL( cellActivated( int,int ) ), this, SLOT( delayPressed( int,int ) ) );
    ui->keyboard->clearContents();
    ui->curMsg->setText(" ");
}

/*** Change voice settings ***/
void MainWindow::setup_voice() {
    curPosition.x = 0;
    curPosition.y = 0;
    ui->title->setText("Voice Options");
    ui->curMsg->show();
    ui->keyboard->setRowCount(8);
    ui->keyboard->setColumnCount(1);

    // Formatting
    ui->keyboard->verticalHeader()->hide();
    ui->keyboard->horizontalHeader()->hide();
    ui->keyboard->horizontalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    ui->keyboard->verticalHeader()->setSectionResizeMode( QHeaderView::Stretch );
    QFont keyFont;
        keyFont.setPointSize( 18 );
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
            if(itemCell->text() == "CANCEL")
                ui->keyboard->item(i, 0)->setBackgroundColor(QColor(230, 50, 50));
            else if(itemCell->text() == "ACCEPT")
                ui->keyboard->item(i, 0)->setBackgroundColor(QColor(90, 200, 90));
            else
                ui->keyboard->item(i, 0)->setBackgroundColor(QColor(240, 240, 255));
            qDebug() << "got here";
            itemCell->setText(voice_list[i]);
        }
    ui->curMsg->setText("CURRENT DELAY: " + QString::number(delay) + "ms");
    connect( ui->keyboard, SIGNAL( cellActivated( int,int ) ), this, SLOT( voicePressed( int,int ) ) );
}
void MainWindow::destroy_voice() {
    disconnect( ui->keyboard, SIGNAL( cellActivated( int,int ) ), this, SLOT( voicePressed( int,int ) ) );
    ui->keyboard->clearContents();
}
/**** Menu slots ****/

/* Menu item selection */
void MainWindow::menuPressed(int row, int column) {
    QString cmd = menu_list[row];
    if(cmd == "POWER OFF") { /* power off device */
        system("sudo shutdown now");
    }
    else if (cmd == "CHANGE WORDS") { /* customize phrases */
        change_phrases();
    }
    else if(cmd == "CHANGE INPUT DELAY") { /* set new input delay */
        destroy_menu();
        setup_delay();
    }
    else if(cmd == "CHANGE VOICE") { /* set new input delay */
        destroy_menu();
        setup_voice();
    }
    else if(cmd == "RETURN") { /* exit the menu */
        destroy_menu();
        setup_keyboard();
    }
}

/* Edit phrase selection */
void MainWindow::custom_itemPressed(int row, int column) {
    QString cmd = item_list[row][column];
    if( cmd == "KEYBOARD") { /* Go back to menu */
        destroy_custom_phrases();
        setup_menu();
    }
    else if (cmd == "") {}
    else {               /* Phrase selected */
        which_word = (row*5)+ column; /* index of phrase to change */
        set_phrase();
    }
}

/* Write new phrase */
void MainWindow::custom_keyPressed(int row, int column) {
    QString cmd = customKeyboard[row][column];
    if(cmd == "SPACE") {
        if( word.size() > 0 ) {
            msg << word ;
            word.clear();
            clear_words();
        }
    }
    else if(cmd == "BACKSPACE") {
        if( word.size() > 0 ) {
            word.remove( ( word.size() - 1 ), 1 );
            update_words( word );
        }
        /* Handle backspace-ing to previous word */
        else if (msg.size() > 0) {
            word = msg.takeLast();
            update_words( word );
        }
    }
    else if (cmd == "CLEAR") {
        word.clear();
        msg.clear();
        clear_words();
    }
    else if (cmd == "ACCEPT") {
        msg << word;               /* done typing - update phrase list */
        QString phrase = msg.join(" ");
        update_phrases(phrase);
        word.clear();
        msg.clear();
        destroy_custom_keyboard(); /* go back to menu */
        setup_menu();
    }
    else if (cmd == "CANCEL") {
        destroy_custom_keyboard();
        setup_menu();
    }
    else if (cmd.contains("WORD")) {
        QString tmp = ui->keyboard->item(row, column)->text();
        if(tmp.size() > 0) {
            word.clear();
            msg << tmp;
            clear_words();
        }
    }
    else if(cmd == "NULL") { }   // Do nothing, shouldn't ever happen anyway
    else {                       // Just add a character to current word
        word.append( cmd );
        update_words( word );
    }
    QString tmp = msg.join(" ");
    tmp.append(" ");
    tmp.append(word);
    ui->curMsg->setText(tmp);
}

/* Configure input delay */
void MainWindow::delayPressed(int row, int column) {
    QString cmd = delay_list[row];
    int tmp = delay;
    if(cmd == "INCREASE DELAY") { /* increase delay by 100ms */
        delay += 100;
    }
    else if (cmd == "DECREASE DELAY") { /* decrease delay by 100ms */
        if(delay >= 100)
            delay -= 100;
    }
    else if(cmd == "TEST") { /* test input delay */
        for(int i=0; i<ui->keyboard->rowCount(); i++) {
            if(ui->keyboard->item(i, 0)->backgroundColor() == QColor(0, 200, 100))
                ui->keyboard->item(i, 0)->setBackgroundColor(QColor(200, 0, 10));
            else
                ui->keyboard->item(i, 0)->setBackgroundColor(QColor(0, 200, 100));
        }
    }
    else if(cmd == "ACCEPT") { /* set new input delay */
        QFile f("/home/gakers/txt2speech/delay.settings");
        f.open(QIODevice::WriteOnly);
        QTextStream out(&f);
        out << delay;
        f.close();
        destroy_delay();
        setup_menu();
    }
    else if(cmd == "CANCEL") { /* exit the menu */
        delay = tmp;
        destroy_delay();
        setup_menu();
    }
    ui->curMsg->setText("CURRENT DELAY: " + QString::number(delay) + "ms");
}

/* Configure voice */
void MainWindow::voicePressed(int row, int column) {
    QString cmd = voice_list[row];
    voice tmp = speechVoice;
    if(cmd == "CHANGE VOICE") {
        int k=0;
        for(int i=0; i<voices.size(); i++) {
            if(speechVoice.name == voices[i])
                k=i;
        }
        if(k == voices.size()-1)
            speechVoice.name = voices[0];
        else
            speechVoice.name = voices[k+1];
    }
    else if(cmd == "INCREASE PITCH") {
        speechVoice.pitch += 5;
    }
    else if (cmd == "DECREASE PITCH") {
        if(speechVoice.pitch >= 5)
            speechVoice.pitch -= 5;
    }
    else if(cmd == "INCREASE SPEED") {
        speechVoice.speed += 5;
    }
    else if (cmd == "DECREASE SPEED") {
        if(speechVoice.speed >= 5)
            speechVoice.speed -= 5;
    }
    else if(cmd == "TEST") { /* test input delay */
        QStringList sentence;
        sentence << "hello" << "world";
            text_to_speech(sentence);
    }
    else if(cmd == "ACCEPT") { /* set new input delay */
        QFile f("/home/gakers/txt2speech/voice.settings");
        f.open(QIODevice::WriteOnly);
        QTextStream out(&f);
        out << "NAME " + speechVoice.name + "\n";
        out << "PITCH " + QString::number(speechVoice.pitch) + "\n";
        out << "SPEED " + QString::number(speechVoice.speed) + "\n";
        f.close();
        destroy_voice();
        setup_menu();
    }
    else if(cmd == "CANCEL") { /* exit the menu */
        speechVoice = tmp;
        destroy_voice();
        setup_menu();
    }
    ui->curMsg->setText("VOICE: "+  speechVoice.name+\
                        " PITCH: "+ QString::number(speechVoice.pitch)+\
                        " SPEED: "+ QString::number(speechVoice.speed));
}

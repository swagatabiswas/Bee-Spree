/*
 * @file mainwindow.cpp
 * @brief contains function definitions for MainWindow class
 *
 * Sets  difficulty levels and changes windows when game started
 * and when game over.
 */

#include "mainwindow.h"
#include "gameboard.h"
#include "ui_mainwindow.h"
#include "instructions.h"
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QGraphicsScene>
#include <QPixmap>
#include <QFont>

/*
 * Constructor for the MainWindow class.
 *
 * @param *parent makes this the parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}


/*
 * Function to start game in easy mode. Causes screen to change from
 * start menu to game display. Has no obstacles and no moving enemies.
 */
void MainWindow::easy_game_begin()
{
    board = new GameBoard(this, 15, 3, false, false);
    this->setCentralWidget(board);
}

/*
 * Function to start game in medium mode. Causes screen to change from
 * start menu to game display. Has obstacles and no moving enemies.
 */
void MainWindow::medium_game_begin()
{
    board = new GameBoard(this, 15, 1, false, true);
    this->setCentralWidget(board);
}

/*
 * Function to start game in hard mode. Causes screen to change from
 * start menu to game display. Has obstacles and moving enemy.
 */
void MainWindow::hard_game_begin()
{
    board = new GameBoard(this, 15, 1);
    this->setCentralWidget(board);
}

/*Function to display gameover message
 *
 * Message is displayed when bee runs into opponent.
 * Function changes central widget from the GameBoard to
 * the gameover screen.
*/
void MainWindow::game_over()
{
    //make exit window the main widget
    QWidget* exit = new QWidget;
    exit->setParent(nullptr);

    QFont endFont("Impact", 20);

    QLabel* message = new QLabel;
    message->setText("Game over! You've failed the hive.");
    message->setFont(endFont);
    message->setAlignment(Qt::AlignCenter);

    //add score to end screen
    QString msg = "Score: ";
    msg += QString::number(board->score);
    QLabel* score_message = new QLabel;
    score_message->setText(msg);
    score_message->setFont(endFont);
    score_message->setAlignment(Qt::AlignCenter);

    QString bee_fileName(":/image/sad_bee.jpg");
    QPixmap* bee_image = new QPixmap(bee_fileName);
    QLabel* bee_pos = new QLabel;
    bee_pos->setPixmap(*bee_image);
    bee_pos->setScaledContents(true);
    bee_pos->setFixedHeight(500);
    bee_pos->setFixedWidth(500);
    //bee_pos->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    //bee_pos->setAlignment(Qt::AlignCenter);


    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(message);
    layout->addWidget(score_message);
    layout->addWidget(bee_pos);
    exit->setLayout(layout);

    ui->setupUi(this);

    this->setCentralWidget(exit);

    //ui->setupUi(this);

}

/*
 * Destructor for MainWindow class. Deletes ui pointer.
 */
MainWindow::~MainWindow()
{
    delete ui;
}

/*Function to display instructions. When instructions button
 *clicked, new dialogue screen is opened that displays instructions.
 * MainWindow cannot be accessed until instructions screen is closed.
*/
void MainWindow::on_pushButton_4_clicked()
{
    Instructions instruc;
    instruc.setModal(true);
    instruc.exec();
}

/*
 * @file mainwindow.h
 * @brief header file to contain MainWindow declaration
 *
 * This headerfile contains the class declaration of the MainWindow class,
 * which manages the main window that is created and that can hold widgets.
 *
 * New public slots defined are easy_game_begin(), medium_game_begin(), and
 * hard_game_begin() which start the game at different difficulty levels.
 *
 * New private variable board sets up a GameBoard object.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gameboard.h"
#include "instructions.h"

namespace Ui {
class MainWindow;
}

/*
 * @class MainWindow
 * @brief creates a window (widget) that can contain other widgets
 *
 * In addition to the standard MainWindow features, this class has
 * a private member variable board, which is a GameBoard object that
 * sets up the board, which is a grid ful of labels. The class also defines
 * 3 new public slots for the 3 difficulty levels of the game.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT


public slots:

    //different difficulties
    void easy_game_begin();
    void medium_game_begin();
    void hard_game_begin();

    void game_over();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    GameBoard* board;
};


#endif // MAINWINDOW_H

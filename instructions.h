/* @file instructions.h
 * @brief contains class definition for Instructions class
 *
 * This header file defines the Instructions class, which
 * opens a new window and displays the instructions for the game.
 */

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <QDialog>

namespace Ui {
class Instructions;
}

/*
 * @class Instructions
 * @brief opens a new window that displays the instructions for the game
 *
 * This class interacts with the MainWindow- when the instructions button
 * on the main window is clicked, it accesses the instructions class to open
 * a new window and show the instructions.
 */
class Instructions : public QDialog
{
    Q_OBJECT

public:
    //constructor
    explicit Instructions(QWidget *parent = 0);
    //destructor
    ~Instructions();

private:
    Ui::Instructions *ui;
};

#endif // INSTRUCTIONS_H

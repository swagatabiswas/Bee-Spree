/* @file instructions.cpp
 * @brief contains the function definitions of the Instructions
 * class.
 *
 * Class contains only a constructor which displays the instructions
 * page. Instructions page constructed in instructions.ui
*/

#include "instructions.h"
#include "ui_instructions.h"
#include "mainwindow.h"
#include "gameboard.h"
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>

/* Function to open instructions window and display the instructions.
 *
 * @param parent makes the QDialog object the parent of all the widgets
 * it contains, which are used to compose the instructions page.
 */
Instructions::Instructions(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Instructions)
{
    ui->setupUi(this);
}

/*
 * Destructor for the Instructions class. Deletes the ui pointer,
 * which recursively deletes all children.
 */
Instructions::~Instructions()
{
    delete ui;
}

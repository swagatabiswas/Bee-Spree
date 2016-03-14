/*
 * @file mainwindow.h
 * @author Swagata Biswa
 * @date 26 February 2016
 * @brief contains the int main code
 * @mainpage
 *
 * HW5
 * This file contains the main code to run the program.
 * Unaltered from the version supplied by Qt.
 */


#include "mainwindow.h"
#include <QApplication>
#include <QLabel>
#include <QWidget>

int main(int argc, char *argv[])
{   
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

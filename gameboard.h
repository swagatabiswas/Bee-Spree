/*
 * @file gameboard.h
 * @brief header file to contain GameBoard class declarations
 *
 * This headerfile contains the class declaration of the GameBoard class, which
 * creates a game board with a 10x10 grid. The class manages several images that
 * can travel around the board and interact with each other.
*/

#ifndef GAMEBOARD_H
#define GAMEBOARD_H

#include <QMainWindow>
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPaintEvent>
#include <QKeyEvent>
#include <vector>
#include <QProgressBar>

namespace Ui {
class GameBoard;
}

/*
 * @class GameBoard
 * @brief sets up the game display and defines how the components of
 * the game interact with each other.
 */
class GameBoard : public QWidget
{
    Q_OBJECT

signals:
    void game_over();

public slots:
    void move_enemy();

public:
    explicit GameBoard(QWidget *parent = 0, size_t board_size = 15, int opp_time = 5, bool moving_enemies = true, bool obstacles = true);
    ~GameBoard();
    void paintEvent(QPaintEvent *e);
    void showEvent(QShowEvent *e);
    void keyPressEvent(QKeyEvent *event);


    //functions to draw/move the elements on the board
    void moveBee(int prev_x, int prev_y, int next_x, int next_y);
    void setFlower();
    void drawOpp();

    void create_enemy();
    void enemy_coordinates();

    size_t score;

private:
    Ui::GameBoard *ui;

    //graphics
    QPixmap* bee_image;
    QPixmap* hive_image;
    QPixmap* flower_image;
    QPixmap* opp_image; //opponent (factory)
    QPixmap* cloud_image;
    QPixmap* obstacle_image;

    //positions of characters
    QPoint* bee_position;
    QPoint* hive_position;
    QPoint* flower_position;
    QPoint* opp_position;
    QPoint* cloud_position;
    QPoint* obstacle_position;

    //displays at top of screen
    QProgressBar* progress;
    QLabel* fullMessage; //displays message when progress bar full
    QLabel* scoreMessage;
    QPixmap* scoreImage;

    //vector to store all opp positions
    std::vector<QPoint*> vector_oppPositions;
    std::vector<QPoint*> vector_obstaclePositions;
    std::vector<QPoint*> vector_cloudPositions;

    size_t counter; //number of flowers visited
    //size_t score;
    size_t num_opps; //number of opponents

    //Board variables
    QWidget* Board;
    size_t board_size;
    QLabel** labels;
    int opp_time; //determines difficulty

    bool moving_enemies; //whether there's moving enemy
    bool obstacles; //whether there are obstacles

};

#endif // GAMEBOARD_H

/*
 * @file gameboard.cpp
 * @brief contains class definition of GameBoard class
 *
 * This file defines the GameBoard class and sets up the
 * board. The bee and hive start in diagonally opposite corners.
 * The opponents appear around the board and can chase the bee.
 */

#include "gameboard.h"
#include "ui_gameboard.h"
#include <mainwindow.h>
#include <QPushButton>
#include <QPainter>
#include <QCoreApplication>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <QProgressBar>
#include <QHBoxLayout>
#include <chrono>
#include <random>
#include <QTimer>
#include <QDebug>
#include <QString>

#include <QFont>
#include <QMultimedia>


unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator (seed);

/*
 * Constructor for the GameBoard class.
 *
 * @param parent sets GameBoard a parent widget
 * @param board_sz is size of board
*/
GameBoard::GameBoard(QWidget *parent, size_t board_sz, int tm, bool moving_enem, bool obst) :
    QWidget(parent),
    ui(new Ui::GameBoard), board_size(board_sz), opp_time(tm), moving_enemies(moving_enem), obstacles(obst)
{
    ui->setupUi(this);

    counter = 0;
    score = 0;
    num_opps = 0;

    //open bee and hive images, position set later
    QString fileName(":/image/bee.jpg");
    bee_image = new QPixmap(fileName);
    QString hive_fileName(":/image/hive.jpg");
    hive_image = new QPixmap(hive_fileName);


    //create the Board
    Board = new QWidget;
    labels = new QLabel*[board_size*board_size]; //square Board
    QGridLayout *SquareGrid = new QGridLayout (Board);

    //make the grid a rectangle (a square)
    SquareGrid->setGeometry(QRect());
    SquareGrid->setSpacing(0);

    //fill SquareGrid with labels in each grid
    for(size_t i = 0; i < board_size; i++)
    {
        for(size_t j = 0; j < board_size; j++)
        {
            //create new label, each with new number
            labels[i*board_size+j] = new QLabel;

            //add label to the layout
            SquareGrid -> addWidget(labels[i*board_size+j] ,i,j);
        }
    }

    //initialize bee at top left corner, always starts at same place
    bee_position = new QPoint(0,0);
    labels[0]->setPixmap(*bee_image);
    labels[0]->setScaledContents(true);
    labels[0]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    //initialize hive to top right corner, always starts at same place
    hive_position = new QPoint(board_size - 1,0);
    labels[board_size - 1]->setPixmap(*hive_image);
    labels[board_size - 1]->setScaledContents(true);
    labels[board_size - 1]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    Board->setFixedSize(500,500);

    //open flower image
    QString flower_fileName(":/image/flower.png");
    flower_image = new QPixmap(flower_fileName);

    //set flower to random place on grid
    this->setFlower();

    //create enemy if correct level
    if(moving_enemies)
        this->create_enemy();

    //open opponent image
    QString opp_fileName(":/image/cloud.png");
    opp_image = new QPixmap(opp_fileName);

    //if on level that has obstacles, open obstacle image
    if(obstacles)
    {
        QString obstacle_fileName(":/image/factory.png");
        obstacle_image = new QPixmap(obstacle_fileName);
    }

    QVBoxLayout *game_layout = new QVBoxLayout;

    //font is Impact, used for score
    QFont impact("Impact", 14);
    QFont headerFont("Impact", 20);

    //add header at top of game
    QLabel* header = new QLabel;
    header->setText("Keep the hive alive!");
    header->setFont(headerFont);
    header->setAlignment(Qt::AlignCenter);
    game_layout->addWidget((header));

    QHBoxLayout *progress_layout = new QHBoxLayout;

    //progress bar to keep track of how much pollen collected
    progress = new QProgressBar;
    progress->setValue(counter); //counter = 0
    progress->setMaximumWidth(100); //make bar smaller
    progress_layout->addWidget(progress);

    //displays message when progress bar full
    fullMessage = new QLabel;
    progress_layout->addWidget(fullMessage);

    QLabel* score_label = new QLabel;
    QString score_filename(":/image/score.png");
    scoreImage = new QPixmap(score_filename);
    score_label->setPixmap(*scoreImage);
    progress_layout->addWidget(score_label);

    scoreMessage = new QLabel;
    scoreMessage->setText(QString::number(score));

    scoreMessage->setFont(impact);
    progress_layout->addWidget(scoreMessage);


    //add progress layout to vertical layout
    game_layout->addLayout(progress_layout);

    //add board to the vertical layout
    game_layout->addWidget(Board,0,Qt::AlignCenter);

    //quit button
    //here, improve by popping up window to check for confirmation
    QPushButton* quit = new QPushButton("Quit");
    quit->setStyleSheet("background-color: darkCyan");
    QObject::connect(quit, SIGNAL(clicked()), parent, SLOT(close()));
    //QObject::connect(quit, SIGNAL(clicked()), this, SLOT(quitDialog()));

    //add quit button to layout
    game_layout->addWidget(quit);

    this->setLayout((game_layout));
    this->setStyleSheet("QLabel { background-color : white;}");

    QObject::connect(this, SIGNAL(game_over()), parent, SLOT(game_over()));

   /*while(!game_over())
    {
        if(player->state() == QMediaPlayer::PlayingState)
        {
            player->setPosition(0);
        }
        else if(player->state() == QMediaPlayer::StoppedState)
        {
            player->play();
        }
   }
*/
}

/*
 *Function that creates a moving enemy. Enemy moves horizontally across screen,
 *based on timer, and if there is collision with bee then game is over. Enemy
 *appears at random spots on screen, new coordinates found using enemy_coordinates
 *function.
*/
void GameBoard::create_enemy()
{
    QString cloud_fileName(":/image/child.jpg");
    cloud_image = new QPixmap(cloud_fileName);

    std::uniform_int_distribution<int> unif(0,board_size - 1);
    int x = unif(generator);
    int y = unif(generator);

    //add to get label number
    int cloud_pos = (y*board_size) + x;

    //set label to display cloud_image
    labels[cloud_pos]->setPixmap(*cloud_image);
    labels[cloud_pos]->setScaledContents(true);
    labels[cloud_pos]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    cloud_position = new QPoint(x,y);
    vector_cloudPositions.push_back(cloud_position);

    QTimer* timer = new QTimer;
    timer->start(100);
    connect(timer, SIGNAL(timeout()), this, SLOT(move_enemy()));
}

/*
 * Function to move enemy around the screen. Done by incrementing x-coordinate
 * of enemy until the end of the board or another object has been reached.
 *
*/
void GameBoard::move_enemy()
{
    int x = cloud_position->rx();
    int y = cloud_position->ry();
    int new_cloud_pos;
    bool end = false; //changes when end reached
    //bool back_to_left = false;

    int cloud_pos = (y*board_size) + x;
    labels[cloud_pos]->clear();

    //check if at rightmost end of board. If reached end, make end = true
    if((x + 1) % board_size == 0 || (x + 1 == flower_position->rx() &&  y == flower_position->ry())
            ||(x + 1 == hive_position->rx()&& y == hive_position->ry()))
    {
        end = true;
    }

    //if reached end of screen, get new coordinates
    if(end)
    {
        enemy_coordinates();
        new_cloud_pos = (board_size* (cloud_position->ry())) + cloud_position->rx();
    }

    if(!end)
        {
            cloud_position->setX(x + 1);
            new_cloud_pos = (board_size* (cloud_position->ry())) + cloud_position->rx();
        }

    //move image to next label
    labels[new_cloud_pos]->setPixmap(*cloud_image);
    labels[new_cloud_pos]->setScaledContents(true);
    labels[new_cloud_pos]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    //if enemy in same position as bee, game over
    if(cloud_position->rx() ==  bee_position->rx() && cloud_position->ry() == bee_position->ry())
    {
        game_over();
    }

    repaint();
}

/*
 * Function to get random coordinates for enemy once end has been reached.
 * Checks to ensure coordinates don't match with coordinates of the other
 * objects on the board.
*/
void GameBoard::enemy_coordinates()
{
    std::uniform_int_distribution<int> unif(0,board_size - 1);

    int new_x = unif(generator);
    int new_y = unif(generator);

    //check if coordinates match with flower, bee, or hive
    while((new_x == flower_position->rx() && new_y == flower_position->ry()) ||
            (new_x == bee_position->rx()&& new_y == bee_position->ry()) ||
            (new_x == hive_position->rx() && new_y == hive_position->ry()))
    {
        new_x = unif(generator);
        new_y = unif(generator);
    }

    cloud_position->setX(new_x);
    cloud_position->setY(new_y);

}

/*
 * Function to draw flowers on the board. Gets random x and y values and makes
 * sure values don't coordinates of the other elements. Then adds image of flower
 * to label corresponding to the coordinates.
 * If enough flowers have been drawn (depending on difficulty) then calls
 * drawOpp() function.
 *
 */
void GameBoard::setFlower()
{   
    //get random number from 0-9 for x and y coordinates
    std::uniform_int_distribution<int> unif(0,board_size - 1);
    int x = unif(generator);
    int y = unif(generator);


    //go through opp positions and make sure flower isn't in same place
    for (size_t i = 0, n = vector_oppPositions.size(); i < n; i++)
    {
        int opp_x = vector_oppPositions[i]->rx();
        int opp_y = vector_oppPositions[i]->ry();

        //same number of obstacles as opps so indices match
        int obstacle_x, obstacle_y;

        if(obstacles)
        {
            obstacle_x = vector_obstaclePositions[i]->rx();
            obstacle_y = vector_obstaclePositions[i]->ry();
        }

        //if no obstacles then just set obstacle's position to same as bee so no problems in following check
        if(!obstacles)
        {
            obstacle_x = bee_position->rx();
            obstacle_y = bee_position->ry();
        }

        //check if flower position matches bee or hive position or the opp/obstacle positions
        if ((x == bee_position->rx() && y == bee_position->ry()) || ((x == board_size - 1) && y == 0)||
                (x == opp_x && y == opp_y) || (x == obstacle_x && y == obstacle_y))
        {
            x = unif(generator);
            y = unif(generator);
        }
    }

    flower_position = new QPoint(x,y);

    //add to get label number
    int flower_pos = (y*board_size) + x;

    //set label to display flower_image
    labels[flower_pos]->setPixmap(*flower_image);
    labels[flower_pos]->setScaledContents(true);
    labels[flower_pos]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    //depending on level, if enough flowers collected, draw opponent
    if(counter % opp_time == 0)
    {
        drawOpp();
    }
}

/*
 * Destructor for GameBoard class.
 * Deletes the ui pointer
 */
GameBoard::~GameBoard()
{
    delete ui;
}

/*
 * Function to paint the board. Redraws the bee every time paintEvent is called.
 * Then redraws hive so if bee is in the same label as hive then the hive shows
 * (as if the bee whent into the hive)
 *
 * @param e is QPaintEvent object called
 */
void GameBoard::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    //painter.setWindow(0,0,100,100);
    painter.setWindow(0,0,board_size - 1, board_size - 1);

    //get x and y coordinates of bee
    int x = bee_position->rx();
    int y = bee_position->ry();

    //redraw bee in new label
    labels[y*board_size+x]->setPixmap(*bee_image);
    labels[y*board_size+x]->setScaledContents(true);
    labels[y*board_size+x]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    //redraw hive so it is always on top in case bee goes to hive (so hive doesn't disappear)
    labels[board_size-1]->setPixmap(*hive_image);
    labels[board_size-1]->setScaledContents(true);
    labels[board_size-1]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    //redraw all the enemies
    for(size_t i = 0, n = vector_oppPositions.size(); i < n; ++i)
    {
        int enemy_x = vector_oppPositions[i]->rx();
        int enemy_y = vector_oppPositions[i]->ry();
        labels[(board_size * enemy_y) + enemy_x]->setPixmap(*opp_image);
        labels[(board_size * enemy_y) + enemy_x]->setScaledContents(true);
        labels[(board_size * enemy_y) + enemy_x]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    }

    //if level has obstacles, redraw all obstacles
    if(obstacles)
    {
        //redraw all the obstacles
        for(size_t i = 0, n = vector_obstaclePositions.size(); i < n; ++i)
        {
            int obstacle_x = vector_obstaclePositions[i]->rx();
            int obstacle_y = vector_obstaclePositions[i]->ry();
            labels[(board_size * obstacle_y) + obstacle_x]->setPixmap(*obstacle_image);
            labels[(board_size * obstacle_y) + obstacle_x]->setScaledContents(true);
            labels[(board_size * obstacle_y) + obstacle_x]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        }
    }

    //draw flower
    int flower_x = flower_position->rx();
    int flower_y = flower_position->ry();
    labels[(board_size * flower_y) + flower_x]->setPixmap(*flower_image);
    labels[(board_size * flower_y) + flower_x]->setScaledContents(true);
    labels[(board_size * flower_y) + flower_x]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

/*
 * Function to move bee to new label
 * Checks conditions to make sure it is a valid move; otherwise
 * displays gameover message.
 * If bee moves to same label as a flower, updates counter and
 * progress bar.
 *
 * @param prev_x is x coordinate of bee before it moves
 * @param prev_y is y coordinate of bee before it moves
 * @param next_x is x coordinate of bee after it moves
 * @param next_y is y coordinate of bee after it moves
 */
void GameBoard::moveBee(int prev_x, int prev_y, int next_x, int next_y)
{
    //if new coordinates same as flower coordinates then increment count
    if (next_x == flower_position->rx() && next_y == flower_position->ry())
    {
        counter++;
        //increase progress bar by 10%
        progress->setValue(counter * 10);
    }

    //if progress bar full, show fullMessage
    if(progress->value() == 100)
    {
        fullMessage->setText("Time to visit the hive!");
        //QFont("Impact", 12);
        fullMessage->setFont(QFont("Impact", 10));
        fullMessage->setStyleSheet("QLabel { color : red;}");
    }

    //otherwise no message
    else
    {
        fullMessage->setText(" ");
    }

    //bee can't move if there is an obstacle
    if(obstacles)
    {
        for(size_t i = 0, n = vector_obstaclePositions.size(); i < n; i++)
        {
            if (next_x == vector_obstaclePositions[i]->rx() && next_y == vector_obstaclePositions[i]->ry())
               {
                bee_position->setX(prev_x);
                bee_position->setY(prev_y);
                return;
               }
        }
    }

    //clear bee's position
    labels[prev_y*board_size+prev_x]->clear();

    //change coordinates of bee
    bee_position->setX(next_x);
    bee_position->setY(next_y);

    //if new coordinates same as flower, have to set new flower
    if (next_x == flower_position->rx() && next_y == flower_position->ry())
    {
        setFlower();
    }

    //if bee in same position as opp, game over
    for(size_t i = 0, n = vector_oppPositions.size(); i < n; i++)
    {
        if (next_x == vector_oppPositions[i]->rx() && next_y == vector_oppPositions[i]->ry())
           {
            this->game_over();
           }
    }

    //if be runs into moving cloud then game over
    for(size_t i = 0, n = vector_cloudPositions.size(); i < n; i++)
    {
        if (next_x == vector_cloudPositions[i]->rx() && next_y == vector_cloudPositions[i]->ry())
        {
            this->game_over();
        }
    }


    //if bee in hive and enough has pollen, increase progress bar
    //if progress bar full, dump pollen
    if(next_x == hive_position->rx() && next_y == hive_position->ry() && progress->value() == 100)
    {
        counter = counter % 10;

        //reset progress bar
        progress->setValue(0);

        //increase score, update score display
        ++score;
        scoreMessage->setText(QString::number(score));

        //if score big enough, get rid of some clouds
        if(score % 1 == 0 && score != 0)
        {
            int x, y;
            int obstacle_x, obstacle_y;

            int size = vector_oppPositions.size() - 1;
            int num_removed;

            //if easy level, remove 1 enemy
            if(!obstacles && !moving_enemies)
                num_removed = 1;
            if(obstacles && !moving_enemies)
                num_removed = 2;
            if(obstacles && moving_enemies)
                num_removed = 4;

            //get rid of clouds, number depends on level
            for(size_t i = 0; i < num_removed; i++)
            {
                //remove opp image from the board
                x = vector_oppPositions[size - i]->rx();
                y = vector_oppPositions[size - i]->ry();
                labels[board_size*y + x]->clear();

                //remove opp's point from the vector
                vector_oppPositions.pop_back();

                //if level has obstacles, also remove obstacles
                if(obstacles)
                {
                    //remove obstacle image from the board
                    obstacle_x = vector_obstaclePositions[size - i]->rx();
                    obstacle_y = vector_obstaclePositions[size - i]->ry();
                    labels[board_size*obstacle_y + obstacle_x]->clear();

                    //remove opp's point from the vector
                    vector_obstaclePositions.pop_back();
                }
            }

            repaint();
        }

    }
}

/*
 * Function to draw the green clouds (opponents). Checks if
 * enough flowers have been collected to draw another opponent.
 * Gets random coordinates and ensures they don't match coordinates
 * of other elements, then draws the opponent on the board.
 */
void GameBoard::drawOpp()
{
    //used to get random coordinates
    std::uniform_int_distribution<int> unif(0,board_size - 1);

    //if right amount of time has passed, draw a new opp
    if (counter == 0)
        return;

    //frequency of opp appearance depends on level (opp_time)
    if(counter % opp_time == 0)
    {
        //get random coordinates for opp and obstacle
        int x = unif(generator);
        int y = unif(generator);
        int obstacle_x = unif(generator);
        int obstacle_y = unif(generator);

        //if no obstacles then just set obstacle coordinates same as bee
        if(!obstacles)
        {
            obstacle_x = bee_position->rx();
            obstacle_y = bee_position->ry();

        }

        //if level has obstacles, make sure coordinates don't match
        if(obstacles)
        {
            //go through vector of opps and check if coordinates match
            for(size_t i = 0, n = vector_oppPositions.size(); i < n; i++)
            {
                int opp_x = vector_oppPositions[i]->rx();
                int opp_y = vector_oppPositions[i]->ry();

                if(obstacle_x == opp_x && obstacle_y == opp_y)
                {
                    obstacle_x = unif(generator);
                    obstacle_y = unif(generator);
                }
            }

            //check if obstacle matches coordinates of other objects
            while ((obstacle_x == bee_position->rx() && obstacle_y == bee_position->ry()) || (obstacle_x == hive_position->rx()
            && obstacle_y == hive_position->ry()) || (obstacle_x == flower_position->rx() && obstacle_y == flower_position->ry()))
            {
                obstacle_x = unif(generator);
                obstacle_y = unif(generator);
            }
        }

        //go through vector of obstacles and make sure coordinates don't match w/ opp
        if(obstacles)
        {
            for(size_t i = 0, n = vector_obstaclePositions.size(); i < n; i++)
            {
                int ob_x = vector_obstaclePositions[i]->rx();
                int ob_y = vector_obstaclePositions[i]->ry();

                //if coordinates match, get new coordinates
                if(x == ob_x && y == ob_y)
                {
                    x = unif(generator);
                    y = unif(generator);
                }
            }
        }

        //make sure coordinates not same as other objects
        while ((x == bee_position->rx() && y == bee_position->ry()) || (x == hive_position->rx()
        && y == hive_position->ry()) || (x == flower_position->rx() && y == flower_position->ry())
               || (x == obstacle_x && y == obstacle_y))
        {
            x = unif(generator);
            y = unif(generator);
        }

        int opp_pos = y*board_size + x;
        int obstacle_pos = obstacle_y*board_size + obstacle_x;

        //set label to display opp_image
        labels[opp_pos]->setPixmap(*opp_image);
        labels[opp_pos]->setScaledContents(true);
        labels[opp_pos]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

        //if level has obstacles, display obstacle
        if(obstacles)
        {
            //set label to display obstacle_image
            labels[obstacle_pos]->setPixmap(*obstacle_image);
            labels[obstacle_pos]->setScaledContents(true);
            labels[obstacle_pos]->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

            obstacle_position = new QPoint(obstacle_x,obstacle_y);
            vector_obstaclePositions.push_back((obstacle_position));
        }

        opp_position = new QPoint(x,y);
        vector_oppPositions.push_back((opp_position));

        num_opps++;

    }
}


/*
 * Function that responds to arrow keys being pressed. Moves bee accordingly
 *
 * @param event is key being pressed
 */
void GameBoard::keyPressEvent(QKeyEvent *event)
{
    this->setFocus();

    //get x and y coordinates of bee
    int x = bee_position->rx();
    int y = bee_position->ry();

    switch (event->key()) {
    case Qt::Key_Left:
        if(x != 0)
        {
            this->moveBee(x,y, x-1, y);
        }

        break;
    case Qt::Key_Right:
        //rightPressed = true;
            if(x != board_size-1)
            {
                this->moveBee(x,y, x+1, y);
            }
        break;
    case Qt::Key_Up:
        if(y != 0)
        {
            this->moveBee(x,y, x, y-1);
        }
        break;
    case Qt::Key_Down:
        if(y != board_size-1)
        {
            this->moveBee(x,y, x, y+1);
        }

        break;

    default:
        QWidget::keyPressEvent(event);
    }

    repaint();
    QCoreApplication::processEvents();
}

/*
 * Function to show the effects of paintEvent and keyPressEvent on the board.
 *
 * @param e processes events and shows them on board.
 */
void GameBoard::showEvent(QShowEvent *e) {

    this->activateWindow();
    this->setFocus();
    QWidget::showEvent(e);
}

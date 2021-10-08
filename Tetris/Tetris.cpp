#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <ctime>
#include <fstream>
#include <string>
#include <vector> 

using namespace std;
using namespace sf;

// public game parametrs
//Window size
const VideoMode gameFieldSize(401, 501);
// game field & buffer
vector<vector<RectangleShape>> gameFieldVisual(20, vector<RectangleShape>(10));
vector<vector<RectangleShape>> gameFieldVisualBufer(20, vector<RectangleShape>(10));
// game field in bool type
vector<vector<bool>> squareOnPosition(20, vector<bool>(10, false));
vector<vector<bool>> squareOnPositionBufer(20, vector<bool>(10, false));
//Figures
// const tables: dificulty & figures
const vector<vector<vector<vector<bool>>>> figures = {
    // O
    {
        //start(rotate not exist)
        {
        {true, true,   false, false},
        {true, true,   false, false},
        {false, false, false, false},
        {false, false, false, false}
        }
    },
    // S
    {
        // start
        {
        {false, true,  true,  false},
        {true,  true,  false, false},
        {false, false, false, false},
        {false, false, false, false}
        },
    //rotate (1)
        {
        {true,  false, false, false},
        {true,  true,  false, false},
        {false, true,  false, false},
        {false, false, false, false}
        }
    },
    // Z
    {
        //start
            {
            {true,  true,  false, false},
            {false, true,  true,  false},
            {false, false, false, false},
            {false, false, false, false}
            },
    //rotate 1
        {
        {false, false, true,  false},
        {false, true,  true,  false},
        {false, true,  false, false},
        {false, false, false, false}
        }
    },
    // J
    {
        //start
            {
            {true,  false, false, false},
            {true,  true,  true,  false},
            {false, false, false, false},
            {false, false, false, false}
            },
    //rotate 1
        {
        {false, true,  false, false},
        {false, true,  false, false},
        {true,  true,  false, false},
        {false, false, false, false}
        },
    //rotate 2
        {
        {false, false, false, false},
        {true,  true,  true,  false},
        {false, false, true,  false},
        {false, false, false, false}
        },
    //rotate 3
        {
        {false, true,  true,  false},
        {false, true,  false, false},
        {false, true,  false, false},
        {false, false, false, false}
        }
    },
    // L
    {
        //start
           {
           {false, false, true,  false},
           {true,  true,  true,  false},
           {false, false, false, false},
           {false, false, false, false}
           },
    //rotate 1
        {
        {true,  true,  false, false},
        {false, true,  false, false},
        {false, true,  false, false},
        {false, false, false, false}
        },
    //rotate 2
        {
        {false, false, false, false},
        {true,  true,  true,  false},
        {true,  false, false, false},
        {false, false, false, false}
        },
    //rotate 3
        {
        {false, true,  false, false},
        {false, true,  false, false},
        {false, true,  true,  false},
        {false, false, false, false}
        }
    },
    // T
    {
        //start
           {
           {false, true,  false, false},
           {true,  true,  true,  false},
           {false, false, false, false},
           {false, false, false, false}
           },
    //rotate 1
        {
        {false, true,  false, false},
        {false, true,  true,  false},
        {false, true,  false, false},
        {false, false, false, false}
        },
    //rotate 2
        {
        {false, false, false, false},
        {true,  true,  true,  false},
        {false, true,  false, false},
        {false, false, false, false}
        },
    //rotate 3
        {
        {false, true,  false, false},
        {true,  true,  false, false},
        {false, true,  false, false},
        {false, false, false, false}
        }
},
// I
{
    // start
    {
    {false, false, false, false},
    {true,  true,  true,  true },
    {false, false, false, false},
    {false, false, false, false}
    },
    //rotate (1)
        {
        {false, true, false, false},
        {false, true, false, false},
        {false, true, false, false},
        {false, true, false, false}
        }
    }
};
const vector<float> dificultySpeedTable = { 
    0.8, 0.72, 0.63, 0.55, 0.47, 0.38, 0.3, 0.22, 0.13, 0.1, 0.08, 0.08, 0.08, 0.07, 0.07, 0.07, 0.05, 0.05, 0.05, 0.03, 0.03, 0.03, 0.03, 0.03, 0.03, 0.03, 0.03, 0.03, 0.02 
};
//current figure
int currentFigure; // 0 - O; 1 - I; 2 - S; 3 - Z; 4 - L; 5 - J; 6 - T
int currentFigureRotate = 0; // 0 - start position
vector<vector<RectangleShape>> currentFigureVisual(4, vector<RectangleShape>(4));
Color currentFigureColor;
//next figure
int nextFigure; // 0 - O; 1 - I; 2 - S; 3 - Z; 4 - L; 5 - J; 6 - T
vector<vector<RectangleShape>> nextFigureVisual(4, vector<RectangleShape>(4));
Color nextFigureColor;
//Game parametrs
//game variables
int dificulty = 0;
int score = 0;
int clearedLines = 0;
//game status
bool pause = false;
bool lose = false;
bool leaderBoard = false;;
bool musicPlay = true;
bool newFigure;

bool custome_compare(const std::pair<string, int>& p1, const std::pair<string, int>& p2) { return p1.second > p2.second; }

bool CheckPosition(RectangleShape square, Mouse mouse, RenderWindow& localWindow) //Check position of mouse relatively square. Need for interaction with buttons
{
    if ((mouse.getPosition(localWindow).x <= -square.getOrigin().x + square.getPosition().x + square.getSize().x &&
        mouse.getPosition(localWindow).x >= -square.getOrigin().x + square.getPosition().x)
        &&
        (mouse.getPosition(localWindow).y >= -square.getOrigin().y + square.getPosition().y &&
            mouse.getPosition(localWindow).y <= -square.getOrigin().y + square.getPosition().y + square.getSize().y))
        return true;
    return false;
} // For buttons. Check square position relatively
int PositionToCord(RectangleShape square, int type) //Convert position of square to his global cord; type 1 - y, type 2 - x; 
{
    int position = 0;

    if (type == 1)
        position = -square.getOrigin().y - 1 + square.getPosition().y;
    else if (type == 2)
        position = -square.getOrigin().x - 1 + square.getPosition().x;

    int globalCord;
    globalCord = position / 25;

    if (type == 1 && globalCord > 20)
        return 20;
    else if (type == 1 && globalCord < 0)
        return 0;

    return globalCord;
}

void SpawnFigure(int preparametr) //preparametr: 0 - start/ restart: Create current & next figures, 1 - countinue: Set current figure parametrs of new figure, recreate new figure 
{
    int yLocalCord, xLocalCord;
    int figureColor = rand() % 7;
    currentFigureRotate = 0;
    int randomStartPosition;

    switch (preparametr)
    {
    case 0:

        currentFigure = rand() % 7;
        do
        {
            nextFigure = rand() % 7;
        } while (nextFigure == currentFigure);

        //set color
        switch (figureColor)
        {
        case 0:
            currentFigureColor = Color::Red;
            break;
        case 1:
            currentFigureColor = Color::Yellow;
            break;

        case 2:
            currentFigureColor = Color(107, 142, 35); // green
            break;

        case 3:
            currentFigureColor = Color(30, 144, 255); // blue
            break;

        case 4:
            currentFigureColor = Color(255, 165, 0); // Orange
            break;

        case 5:
            currentFigureColor = Color(75, 0, 130);
            break;
        case 6:
            currentFigureColor = Color(255, 105, 180); // pink
            break;


        }
        figureColor = rand() % 7;
        switch (figureColor)
        {
        case 0:
            nextFigureColor = Color::Red;
            break;
        case 1:
            nextFigureColor = Color::Yellow;
            break;

        case 2:
            nextFigureColor = Color(107, 142, 35); // green
            break;

        case 3:
            nextFigureColor = Color(30, 144, 255); // blue
            break;

        case 4:
            nextFigureColor = Color(255, 165, 0); // Orange
            break;

        case 5:
            nextFigureColor = Color(75, 0, 130);
            break;
        case 6:
            nextFigureColor = Color(255, 105, 180); // pink
            break;


        }

        // set figure elements parametrs
        for (yLocalCord = 0; yLocalCord < currentFigureVisual.size(); yLocalCord++)
            for (xLocalCord = 0; xLocalCord < currentFigureVisual[yLocalCord].size(); xLocalCord++)
            {
                currentFigureVisual[yLocalCord][xLocalCord].setSize(Vector2f(24, 24));
                currentFigureVisual[yLocalCord][xLocalCord].setFillColor(currentFigureColor);
                currentFigureVisual[yLocalCord][xLocalCord].setOrigin(-(1 + xLocalCord * 25), -(1 + yLocalCord * 25));
                currentFigureVisual[yLocalCord][xLocalCord].setPosition(0, 0);
                currentFigureVisual[yLocalCord][xLocalCord].move(0, -25);
                cout << "Figure parametrs:\nSize: " << currentFigureVisual[yLocalCord][xLocalCord].getSize().x << "x" << currentFigureVisual[yLocalCord][xLocalCord].getSize().y << "Position: " << -currentFigureVisual[yLocalCord][xLocalCord].getOrigin().x + currentFigureVisual[yLocalCord][xLocalCord].getPosition().x << "x" << -currentFigureVisual[yLocalCord][xLocalCord].getOrigin().y + currentFigureVisual[yLocalCord][xLocalCord].getPosition().y << endl;
            }

        for (yLocalCord = 0; yLocalCord < currentFigureVisual.size(); yLocalCord++)
            for (xLocalCord = 0; xLocalCord < currentFigureVisual[yLocalCord].size(); xLocalCord++)
            {
                nextFigureVisual[yLocalCord][xLocalCord].setSize(Vector2f(24, 24));
                nextFigureVisual[yLocalCord][xLocalCord].setFillColor(nextFigureColor);
                nextFigureVisual[yLocalCord][xLocalCord].setOrigin(-(1 + xLocalCord * 25), -(1 + yLocalCord * 25));
                nextFigureVisual[yLocalCord][xLocalCord].setPosition(261, 280);
                if (nextFigure == 6)
                    nextFigureVisual[yLocalCord][xLocalCord].move(0, -25);
            }

        //rand pos
        switch (currentFigure)
        {
        case 0:
            randomStartPosition = rand() % 9;
            break;
        case 6:
            randomStartPosition = rand() % 7;
            break;
        default:
            randomStartPosition = rand() % 8;
            break;
        }
        for (yLocalCord = 0; yLocalCord < 4; yLocalCord++)
            for (xLocalCord = 0; xLocalCord < 4; xLocalCord++)
                currentFigureVisual[yLocalCord][xLocalCord].move(25 * randomStartPosition, 0);
        break;

    case 1:

        currentFigure = nextFigure;
        do
        {
            nextFigure = rand() % 7;
        } while (nextFigure == currentFigure);
        currentFigureColor = nextFigureColor;
        switch (figureColor)
        {
        case 0:
            nextFigureColor = Color::Red;
            break;
        case 1:
            nextFigureColor = Color::Yellow;
            break;

        case 2:
            nextFigureColor = Color(107, 142, 35); // green
            break;

        case 3:
            nextFigureColor = Color(30, 144, 255); // blue
            break;

        case 4:
            nextFigureColor = Color(255, 165, 0); // Orange
            break;

        case 5:
            nextFigureColor = Color(75, 0, 130);
            break;
        case 6:
            nextFigureColor = Color(255, 105, 180); // pink
            break;
        }

        currentFigureVisual = nextFigureVisual;

        for (yLocalCord = 0; yLocalCord < currentFigureVisual.size(); yLocalCord++)
            for (xLocalCord = 0; xLocalCord < currentFigureVisual[yLocalCord].size(); xLocalCord++)
                currentFigureVisual[yLocalCord][xLocalCord].setPosition(0, -25);

        for (yLocalCord = 0; yLocalCord < currentFigureVisual.size(); yLocalCord++)
            for (xLocalCord = 0; xLocalCord < currentFigureVisual[yLocalCord].size(); xLocalCord++)
            {
                nextFigureVisual[yLocalCord][xLocalCord].setFillColor(nextFigureColor);
                if (nextFigure == 6)
                    nextFigureVisual[yLocalCord][xLocalCord].move(0, -25);
                else if (currentFigure == 6)
                    nextFigureVisual[yLocalCord][xLocalCord].move(0, 25);
            }

        switch (currentFigure)
        {
        case 0:
            randomStartPosition = rand() % 9;
            break;
        case 6:
            randomStartPosition = rand() % 7;
            break;
        default:
            randomStartPosition = rand() % 8;
            break;
        }
        for (yLocalCord = 0; yLocalCord < 4; yLocalCord++)
            for (xLocalCord = 0; xLocalCord < 4; xLocalCord++)
                currentFigureVisual[yLocalCord][xLocalCord].move(25 * randomStartPosition, 0);
        break;

    default:
        break;
    }

}
bool FigureMoveCheck(int type) // Checks if the shape can move in the selected direction; type 1 - right, type 2 - left
{
    int xLocal, yLocal;
    bool skip = false;
    switch (type)
    {
    case 1:
        for (yLocal = 0; yLocal < currentFigureVisual.size(); yLocal++)
            for (xLocal = currentFigureVisual[yLocal].size() - 1; xLocal >= 0; xLocal--)
                if (figures[currentFigure][currentFigureRotate][yLocal][xLocal] && PositionToCord(currentFigureVisual[yLocal][xLocal], 2) < 10)
                    if (PositionToCord(currentFigureVisual[yLocal][xLocal], 2) + 1 >= 10 ||
                        squareOnPosition[PositionToCord(currentFigureVisual[yLocal][xLocal], 1)][PositionToCord(currentFigureVisual[yLocal][xLocal], 2) + 1])
                        return false;
        break;
    case 2:
        for (yLocal = 0; yLocal < currentFigureVisual.size(); yLocal++)
            for (xLocal = 0; xLocal < currentFigureVisual[yLocal].size(); xLocal++)
                if (figures[currentFigure][currentFigureRotate][yLocal][xLocal] && PositionToCord(currentFigureVisual[yLocal][xLocal], 2) >= 0)
                    if (PositionToCord(currentFigureVisual[yLocal][xLocal], 2) - 1 < 0 ||
                        squareOnPosition[PositionToCord(currentFigureVisual[yLocal][xLocal], 1)][PositionToCord(currentFigureVisual[yLocal][xLocal], 2) - 1])
                        return false;
        break;

    default:
        break;
    }
    return true;
}
bool FigureRotateCheck(int rotateCoeficient) //Checks if a figure can rotate. rotate coeficient - next currentFigureRotate parametr (currentFigureRotate - rotateCoeficient = next rotate)
{
    int yLocal, xLocal;

    for (yLocal = 0; yLocal < currentFigureVisual.size(); yLocal++) // right
        for (xLocal = 0; xLocal < currentFigureVisual[yLocal].size(); xLocal++)
            if (figures[currentFigure][currentFigureRotate + rotateCoeficient][yLocal][xLocal])
                if ((PositionToCord(currentFigureVisual[yLocal][xLocal], 1) >= gameFieldVisual.size() || PositionToCord(currentFigureVisual[yLocal][xLocal], 1) < 0)
                    ||
                    (PositionToCord(currentFigureVisual[yLocal][xLocal], 2) >= gameFieldVisual[yLocal].size() || PositionToCord(currentFigureVisual[yLocal][xLocal], 2) < 0))
                    return false;

    //down
    for (xLocal = 0; xLocal < currentFigureVisual.size(); xLocal++) // right
        for (yLocal = currentFigureVisual.size() - 1; yLocal >= 0; yLocal--)
            if (figures[currentFigure][currentFigureRotate + rotateCoeficient][yLocal][xLocal])
                if (squareOnPosition[PositionToCord(currentFigureVisual[yLocal][xLocal], 1)][PositionToCord(currentFigureVisual[yLocal][xLocal], 2)])
                    return false;
    //up
    for (xLocal = 0; xLocal < currentFigureVisual.size(); xLocal++) // right
        for (yLocal = 0; yLocal < currentFigureVisual.size(); yLocal++)
            if (figures[currentFigure][currentFigureRotate + rotateCoeficient][yLocal][xLocal])
                if (squareOnPosition[PositionToCord(currentFigureVisual[yLocal][xLocal], 1)][PositionToCord(currentFigureVisual[yLocal][xLocal], 2)])
                    return false;

    //right
    for (yLocal = 0; yLocal < currentFigureVisual.size(); yLocal++) // right
        for (xLocal = currentFigureVisual[yLocal].size() - 1; xLocal >= 0; xLocal--)
            if (figures[currentFigure][currentFigureRotate + rotateCoeficient][yLocal][xLocal])
                if (squareOnPosition[PositionToCord(currentFigureVisual[yLocal][xLocal], 1)][PositionToCord(currentFigureVisual[yLocal][xLocal], 2)])
                    return false;

    //left
    for (yLocal = 0; yLocal < currentFigureVisual.size(); yLocal++) // left
        for (xLocal = 0; xLocal < currentFigureVisual[yLocal].size(); xLocal++)
            if (figures[currentFigure][currentFigureRotate + rotateCoeficient][yLocal][xLocal])
                if (squareOnPosition[PositionToCord(currentFigureVisual[yLocal][xLocal], 1)][PositionToCord(currentFigureVisual[yLocal][xLocal], 2)])
                    return false;

    return true;
}

void FigureMove(int type) // move figure. Type - Direction of movement: 0 - down, 1 - right, 2 - left;
{
    int yLocal, xLocal;
    switch (type)
    {
    case 0:
        for (yLocal = 0; yLocal < currentFigureVisual.size(); yLocal++)
            for (xLocal = 0; xLocal < currentFigureVisual[yLocal].size(); xLocal++)
                currentFigureVisual[yLocal][xLocal].move(0, 25);
        break;
    case 1:
        for (yLocal = 0; yLocal < currentFigureVisual.size(); yLocal++)
            for (xLocal = 0; xLocal < currentFigureVisual[yLocal].size(); xLocal++)
                currentFigureVisual[yLocal][xLocal].move(25, 0);
        break;
    case 2:
        for (yLocal = 0; yLocal < currentFigureVisual.size(); yLocal++)
            for (xLocal = 0; xLocal < currentFigureVisual[yLocal].size(); xLocal++)
                currentFigureVisual[yLocal][xLocal].move(-25, 0);
        break;
    default:
        cout << "This type not exist" << endl;
        break;
    }

}
void FigureRotate() // Rotate figure
{
    switch (currentFigure)
    {
    case 1: // S
        switch (currentFigureRotate)
        {
        case 0:
            if (FigureRotateCheck(1))
                currentFigureRotate++;
            break;
        case 1:
            if (FigureRotateCheck(-1))
                currentFigureRotate = 0;
            break;
        default:
            break;
        }
        break;
    case 2: //
        switch (currentFigureRotate)
        {
        case 0:
            if (FigureRotateCheck(1))
                currentFigureRotate++;
            break;
        case 1:
            if (FigureRotateCheck(-1))
                currentFigureRotate = 0;
            break;
        default:
            break;
        }
        break;
    case 3:
        switch (currentFigureRotate)
        {
        case 0:
            if (FigureRotateCheck(1))
                currentFigureRotate++;
            break;
        case 1:
            if (FigureRotateCheck(1))
                currentFigureRotate++;
            break;
        case 2:
            if (FigureRotateCheck(1))
                currentFigureRotate++;
            break;
        case 3:
            if (FigureRotateCheck(-3))
                currentFigureRotate = 0;
            break;
        default:
            break;
        }
        break;
    case 4:
        switch (currentFigureRotate)
        {
        case 0:
            if (FigureRotateCheck(1))
                currentFigureRotate++;
            break;
        case 1:
            if (FigureRotateCheck(1))
                currentFigureRotate++;
            break;
        case 2:
            if (FigureRotateCheck(1))
                currentFigureRotate++;
            break;
        case 3:
            if (FigureRotateCheck(-3))
                currentFigureRotate = 0;
            break;
        default:
            break;
        }
        break;
    case 5:
        switch (currentFigureRotate)
        {
        case 0:
            if (FigureRotateCheck(1))
                currentFigureRotate++;
            break;
        case 1:
            if (FigureRotateCheck(1))
                currentFigureRotate++;
            break;
        case 2:
            if (FigureRotateCheck(1))
                currentFigureRotate++;
            break;
        case 3:
            if (FigureRotateCheck(-3))
                currentFigureRotate = 0;
            break;
        default:
            break;
        }
        break;
    case 6:
        switch (currentFigureRotate)
        {
        case 0:
            if (FigureRotateCheck(1))
                currentFigureRotate++;
            break;
        case 1:
            if (FigureRotateCheck(-1))
                currentFigureRotate = 0;
            break;
        default:
            break;
        }
        break;
    }
}

void SetFigurePosition() // Set figure to game field
{
    int yLocal, xLocal;
    int yGlobal, xGlobal;
    for (yLocal = 0; yLocal < currentFigureVisual.size(); yLocal++)
        for (xLocal = 0; xLocal < currentFigureVisual[yLocal].size(); xLocal++)
            if (figures[currentFigure][currentFigureRotate][yLocal][xLocal])
            {
                yGlobal = PositionToCord(currentFigureVisual[yLocal][xLocal], 1);
                xGlobal = PositionToCord(currentFigureVisual[yLocal][xLocal], 2);
                gameFieldVisual[yGlobal][xGlobal].setFillColor(currentFigureColor);
                squareOnPosition[yGlobal][xGlobal] = true;
            }
    score += 10;
}
void FigureFall() // Check: figure maybeen fove down? If yes - move figure down, if no - set figure to gamefield
{
    int xLocal, yLocal;
    bool skip = false;
    for (xLocal = 0; xLocal < currentFigureVisual.size(); xLocal++)
    {
        for (yLocal = currentFigureVisual.size() - 1; yLocal >= 0; yLocal--)
        {
            skip = false;
            if (figures[currentFigure][currentFigureRotate][yLocal][xLocal] && PositionToCord(currentFigureVisual[yLocal][xLocal], 1) > 0)
            {
                if (PositionToCord(currentFigureVisual[yLocal][xLocal], 1) + 1 >= 20 ||
                    squareOnPosition[PositionToCord(currentFigureVisual[yLocal][xLocal], 1) + 1][PositionToCord(currentFigureVisual[yLocal][xLocal], 2)])
                    newFigure = true;
                else
                    skip = true;
            }

            if (newFigure || skip)
                break;
        }
        if (newFigure)
            break;
    }
    if (!newFigure)
        FigureMove(0);
    else
        SetFigurePosition();
}

void Restart() // restart game: reset all parametrs & recreate current & new figures, game field
{
    int xGlobalCord, yGlobalCord, yLocalCord, xLocalCord;

    dificulty = 0;
    score = 0;
    clearedLines = 0;

    pause = false;
    lose = false;
    newFigure = false;

    for (yGlobalCord = 0; yGlobalCord < gameFieldVisual.size(); yGlobalCord++)
        for (xGlobalCord = 0; xGlobalCord < gameFieldVisual[yGlobalCord].size(); xGlobalCord++)
        {
            gameFieldVisual[yGlobalCord][xGlobalCord].setSize(Vector2f(24, 24));
            gameFieldVisual[yGlobalCord][xGlobalCord].setFillColor(Color::White);
            gameFieldVisual[yGlobalCord][xGlobalCord].setPosition(1 + xGlobalCord * 25, 1 + yGlobalCord * 25);
            squareOnPosition[yGlobalCord][xGlobalCord] = false;
        }
    gameFieldVisualBufer = gameFieldVisual;
    squareOnPositionBufer = squareOnPosition;


    currentFigure = 0;
    currentFigureColor = Color::White;
    currentFigureRotate = 0;

    nextFigure = 0;
    nextFigureColor = currentFigureColor;

    for (yLocalCord = 0; yLocalCord < currentFigureVisual.size(); yLocalCord++)
        for (xLocalCord = 0; xLocalCord < currentFigureVisual[yLocalCord].size(); xLocalCord++)
        {
            currentFigureVisual[yLocalCord][xLocalCord].setOrigin(0, 0);
            currentFigureVisual[yLocalCord][xLocalCord].setPosition(0, 0);
            currentFigureVisual[yLocalCord][xLocalCord].setSize(Vector2f(0, 0));
            currentFigureVisual[yLocalCord][xLocalCord].setFillColor(Color::White);
        }
    nextFigureVisual = currentFigureVisual;
    SpawnFigure(0);

}
bool NameCheck(int sorted, string checkName, vector<string> playersName)
{
    int i;
    for (i = 0; i < sorted; i++)
        if (playersName[i] == checkName)
            return false;
    return true;
}
string LeaderBoardText(int type )
{
    int playersCount = 0;
    int symbolTextCounter = 0, playerid = 0;
    int symbolCounter = 1;
    ifstream leaderBoard("Resources\\Saves\\leaderboard.txt");
    string fullText = "";
    string scoreBufer = "";
    string finalText = "";
    while(leaderBoard)
        getline(leaderBoard, fullText);
    for (symbolTextCounter = 0; symbolTextCounter < fullText.size(); symbolTextCounter++)
        if (fullText[symbolTextCounter] == '|')
            playersCount++;

    vector<string> leaders(10);
    if(playersCount < 10)
        leaders.resize(playersCount);
    vector<pair<string, int>> playerParametrs(playersCount);

    for (symbolTextCounter = 0; symbolTextCounter < fullText.size(); symbolTextCounter++)
    {
        if (symbolCounter % 2 == 0 && fullText[symbolTextCounter] != '>')
            scoreBufer += fullText[symbolTextCounter];
        else if(fullText[symbolTextCounter] != '>' && fullText[symbolTextCounter] != '|')
           playerParametrs[playerid].first += fullText[symbolTextCounter];

        if (fullText[symbolTextCounter] == '>')
            symbolCounter++;
        else if (fullText[symbolTextCounter] == '|')
        {
            playerParametrs[playerid].second = stoi(scoreBufer);
            scoreBufer = "";
            playerid++;
        }

    }
    sort(playerParametrs.begin(), playerParametrs.end(), custome_compare);

    if (type == 0)
    {
        finalText = "Name\n";
        for (playerid = 0; playerid < leaders.size(); playerid++)
            finalText += to_string(playerid+1) + "." + playerParametrs[playerid].first + "\n";
    }
    else if (type == 1)
    {
        finalText = "Score\n";
        for (playerid = 0; playerid < leaders.size(); playerid++)
            finalText += to_string(playerParametrs[playerid].second) + "\n";
    }
    return finalText;
}
void SaveResult(string name, int score)
{
    int playersCount = 0;
    int symbolTextCounter = 0, playerid = 0;
    int symbolCounter = 1;
    ifstream leaderBoardI("Resources\\Saves\\leaderboard.txt");
    string fullText = "";
    string scoreBufer = "";
    string nameBufer = "";
    while (leaderBoardI)
        getline(leaderBoardI, fullText);
    for (symbolTextCounter = 0; symbolTextCounter < fullText.size(); symbolTextCounter++)
        if (fullText[symbolTextCounter] == '|')
            playersCount++;
    leaderBoardI.close();

    vector<pair<string, int>> playerParametrs(playersCount);
    for (symbolTextCounter = 0; symbolTextCounter < fullText.size(); symbolTextCounter++)
    {
        if (symbolCounter % 2 == 0 && fullText[symbolTextCounter] != '>')
            scoreBufer += fullText[symbolTextCounter];
        else if (fullText[symbolTextCounter] != '>' && fullText[symbolTextCounter] != '|')
            playerParametrs[playerid].first += fullText[symbolTextCounter];

        if (fullText[symbolTextCounter] == '>')
            symbolCounter++;
        else if (fullText[symbolTextCounter] == '|')
        {
            playerParametrs[playerid].second = stoi(scoreBufer);
            scoreBufer = "";
            playerid++;
        }

    }

    for (playerid = 0; playerid < playersCount; playerid++)
        if (name == playerParametrs[playerid].first)
        {
            nameBufer = "";
            symbolCounter = 1;
            for (symbolTextCounter = 0; symbolTextCounter < fullText.size(); symbolTextCounter++)
            {
                if (symbolCounter % 2 == 0 && fullText[symbolTextCounter] != '>')
                    scoreBufer += fullText[symbolTextCounter];
                else if (fullText[symbolTextCounter] != '>' && fullText[symbolTextCounter] != '|')
                    nameBufer += fullText[symbolTextCounter];

                if (fullText[symbolTextCounter] == '>')
                    symbolCounter++;
                else if (fullText[symbolTextCounter] == '|')
                {
                    playerParametrs[playerid].second = stoi(scoreBufer);
                    if (playerParametrs[playerid].second >= score && name == nameBufer)
                        return;
                    else if (nameBufer == name)
                    {
                        ofstream leaderBoardO("leaderboard.txt");
                        symbolTextCounter -= 1;
                        symbolTextCounter -= scoreBufer.size();
                        fullText.erase(symbolTextCounter, scoreBufer.size());
                        fullText.insert(symbolTextCounter, to_string(score));
                        leaderBoardO << fullText;
                        leaderBoardO.close();
                        return;
                    }
                    scoreBufer = "";
                    nameBufer = "";
                }

            }
        }
    ofstream leaderBoardO("Resources\\Saves\\leaderboard.txt", ios_base::app);
    leaderBoardO << name << ">" << score << ">|";
    leaderBoardO.close();
}

int main()
{
    //Variables
    ifstream leaderBoardTest("Resources\\Saves\\leaderboard.txt");
    if (!leaderBoardTest.is_open())
    {
        ofstream leaderBoardCreate("Resources\\Saves\\leaderboard.txt");
        leaderBoardCreate.close();
    }
    leaderBoardTest.close();

    //control
    Mouse mouse;
    //Time
    //figure move
    Clock figureMoveTime;
    Time figureMoveTimer = figureMoveTime.getElapsedTime();
    // delay bofore clicks
    Clock delayBeforClick;
    Time delayBeforClickTimer = delayBeforClick.getElapsedTime();
    //Cord
    // local-global cords
    int yGlobalCord, xGlobalCord, yLocalCord, xLocalCord;
    //
    
    int existInThisSeries = 0;

    int deletedSeries = 0;
    bool deleted = false;
    int deletedPerTick = 0;

    bool figureFallForcibly = false;

    string playerNameEnter;
    //Textures & fonts
    // Fonts (create & load)
    Font font;
    font.loadFromFile("Resources\\Textures\\Font\\Arial.ttf");
    // Textures (create & load)
    Texture pauseIconTexture;
    pauseIconTexture.loadFromFile("Resources\\Textures\\Icon\\PauseIcon.png");
    Texture lederBoardButtonTexture;
    lederBoardButtonTexture.loadFromFile("Resources\\Textures\\Icon\\LeaderBoardIcon.png");
    Texture audioOnTexture;
    audioOnTexture.loadFromFile("Resources\\Textures\\Icon\\Music play.png");
    Texture audioOffTexture;
    audioOffTexture.loadFromFile("Resources\\Textures\\Icon\\Music not play.png");
    // Audio (create & load)
    Music mainTheme;
    mainTheme.openFromFile("Resources\\Audio\\main theme.ogg");
    mainTheme.setLoop(true);
    mainTheme.play();
    SoundBuffer figureSetSoundBufer;
    figureSetSoundBufer.loadFromFile("Resources\\Audio\\set sound.ogg");
    SoundBuffer figureDropSoundBufer;
    figureDropSoundBufer.loadFromFile("Resources\\Audio\\drop sound.ogg");
    Sound figureSetSound;
    figureSetSound.setBuffer(figureSetSoundBufer);
    Sound figureDropSound(figureDropSoundBufer);
    //Text
    //Score
    Text scoreText("0", font, 24);
    scoreText.setFillColor(Color::Black);
    scoreText.setPosition(261, 0);
    //Cleared
    Text clearedLinesText("0", font, 24);
    clearedLinesText.setFillColor(Color::Black);
    clearedLinesText.setPosition(261, 80);
    //Dificulty
    Text levelText("Level \n" + to_string(dificulty), font, 24);
    levelText.setFillColor(Color::Black);
    levelText.setPosition(261, 160);
    //Next figure
    Text nextFigureText("Next figure ", font, 24);
    nextFigureText.setFillColor(Color::Black);
    nextFigureText.setPosition(261, 240);
    // You lose
    Text loseText("\t\t\t  You Lose", font, 30);
    loseText.setFillColor(Color::Black);
    loseText.setPosition(0, 200);
    //leader board  Tile
    Text leaderBoardTile("\t\t   Leader's board", font, 30);
    leaderBoardTile.setFillColor(Color::Black);
    leaderBoardTile.setPosition(0, 0);
    // Leader board
    Text leaderBoardNameText("", font, 30);
    leaderBoardNameText.setFillColor(Color::Black);
    leaderBoardNameText.setPosition(0, 45);
    Text leaderBoardScoreText("", font, 30);
    leaderBoardScoreText.setFillColor(Color::Black);
    leaderBoardScoreText.setPosition(300, 45);
    // my name
    Text myNameText("Your name", font, 24);
    myNameText.setFillColor(Color::Black);
    myNameText.setPosition(0, 250);
    //not save my results text
    Text notSaveMyResultsText("Not save", font, 28);
    notSaveMyResultsText.setFillColor(Color::White);
    notSaveMyResultsText.setPosition(115, 280);
    //save my results text
    Text saveMyResultsText("Save results", font, 24);
    saveMyResultsText.setFillColor(Color::White);
    saveMyResultsText.setPosition(236, 250);
    // player enter name
    Text playerNameEnterText("", font, 18);
    playerNameEnterText.setFillColor(Color::Black);
    playerNameEnterText.setPosition(130, 254);
    // 
    //Buttons & icons
    //Icon
    Image windowIcon;
    windowIcon.loadFromFile("Resources\\Textures\\Icon\\icon.png");
    RectangleShape pauseIcon(Vector2f(24, 24));
    pauseIcon.setPosition(gameFieldSize.width - 24 - 10, gameFieldSize.height - 24 - 5);
    pauseIcon.setTexture(&pauseIconTexture);
    //buttons
    RectangleShape audioOnOffButton(Vector2f(24, 24));
    audioOnOffButton.setPosition(gameFieldSize.width - 48 - 10 - 5, gameFieldSize.height - 24 - 5);
    audioOnOffButton.setTexture(&audioOnTexture);
    // open/close leaderboard
    RectangleShape leaderBoardButton(Vector2f(24, 24));
    leaderBoardButton.setTexture(&lederBoardButtonTexture);
    leaderBoardButton.setPosition(377, 2);
    // save results
    RectangleShape saveMyResultsButton(Vector2f(144, 24));
    saveMyResultsButton.setFillColor(Color(48, 213, 200));
    saveMyResultsButton.setPosition(236, 254);
    // countinue withous save
    RectangleShape notSaveMyResultsButton(Vector2f(144, 24));
    notSaveMyResultsButton.setFillColor(Color(48, 213, 200));
    notSaveMyResultsButton.setPosition(100, 288);
    // name enter field
    RectangleShape myNameEnterField(Vector2f(96, 24));
    myNameEnterField.setFillColor(Color::White);
    myNameEnterField.setPosition(130, 254);
    //Other
    RectangleShape grayFilter(Vector2f(gameFieldSize.width, gameFieldSize.height));
    grayFilter.setFillColor(Color(185, 185, 185, 126));
    grayFilter.setPosition(0, 0);
    //start Parametrs
    //set rand seed
    srand(time(0));
    //create gamefield squares 
    for (yGlobalCord = 0; yGlobalCord < gameFieldVisual.size(); yGlobalCord++)
        for (xGlobalCord = 0; xGlobalCord < gameFieldVisual[yGlobalCord].size(); xGlobalCord++)
        {
            gameFieldVisual[yGlobalCord][xGlobalCord].setSize(Vector2f(24, 24));
            gameFieldVisual[yGlobalCord][xGlobalCord].setFillColor(Color::White);
            gameFieldVisual[yGlobalCord][xGlobalCord].setPosition(1 + xGlobalCord * 25, 1 + yGlobalCord * 25);
        }
    // create window
    RenderWindow gameWindow(gameFieldSize, "Tetris");
    gameWindow.setIcon(32, 32, windowIcon.getPixelsPtr());
    // create figure
    SpawnFigure(0);
    // restart clocks
    delayBeforClick.restart();
    figureMoveTime.restart();
    //Start
    while (gameWindow.isOpen())
    {
        Event gameEvent;
        // update variables
        if (!pause && !lose && !leaderBoard)
        {
            newFigure = false;
            figureFallForcibly = false;
            delayBeforClickTimer = delayBeforClick.getElapsedTime();
            figureMoveTimer = figureMoveTime.getElapsedTime();
        }

        while (gameWindow.pollEvent(gameEvent))
        {
            //close game
            if (gameEvent.type == Event::Closed)
                gameWindow.close();
            // keyboard control
            if (gameEvent.type == Event::KeyPressed)
            {
                if (!leaderBoard)
                {
                    //pause
                    if ((gameEvent.key.code == Keyboard::P || gameEvent.key.code == Keyboard::Pause || gameEvent.key.code == Keyboard::Escape) && !pause && ((dificulty >= 29 && delayBeforClickTimer.asSeconds() >= dificultySpeedTable[28] + 0.01) || delayBeforClickTimer.asSeconds() >= dificultySpeedTable[dificulty] + 0.01))
                    {
                        pause = true;
                        delayBeforClick.restart();
                    }
                    //unpause
                    else if ((gameEvent.key.code == Keyboard::P || gameEvent.key.code == Keyboard::Pause || gameEvent.key.code == Keyboard::Escape) && pause && delayBeforClickTimer.asSeconds() >= 0.1)
                    {
                        pause = false;
                        delayBeforClick.restart();
                        figureMoveTime.restart();
                    }
                    // Figure Control
                    if (!pause && !lose)
                    {
                        switch (gameEvent.key.code)
                        {
                            //down
                        case Keyboard::Down:
                            FigureFall();
                            figureFallForcibly = true;
                            delayBeforClick.restart();
                            figureMoveTime.restart();
                            break;
                            //right
                        case Keyboard::Right:
                            if (FigureMoveCheck(1))
                                FigureMove(1);
                            delayBeforClick.restart();
                            break;
                            //left
                        case Keyboard::Left:
                            if (FigureMoveCheck(2))
                                FigureMove(2);
                            delayBeforClick.restart();
                            break;
                            //Rotate
                        case Keyboard::Up:
                            FigureRotate();
                            delayBeforClick.restart();
                            break;
                            // duplicate for WASD control
                        //down
                        case Keyboard::S:
                            FigureFall();
                            figureFallForcibly = true;
                            delayBeforClick.restart();
                            figureMoveTime.restart();
                            break;
                            //right
                        case Keyboard::D:
                            if (FigureMoveCheck(1))
                                FigureMove(1);
                            delayBeforClick.restart();
                            break;
                            //left
                        case Keyboard::A:
                            if (FigureMoveCheck(2))
                                FigureMove(2);
                            delayBeforClick.restart();
                            break;
                            //Rotate
                        case Keyboard::W:
                            FigureRotate();
                            delayBeforClick.restart();
                            break;

                            //Drop
                        case Keyboard::Space:
                            do
                            {
                                FigureFall();
                            } while (!newFigure);
                            delayBeforClick.restart();
                            break;
                            //Key not  provided
                        default:
                            break;
                        }
                    }
                }
                else
                    if (gameEvent.key.code == Keyboard::Escape && leaderBoard && delayBeforClickTimer.asSeconds() >= 0.1)
                    {
                        leaderBoard = false;
                        delayBeforClick.restart();
                    }
            }
            if (lose)
                if (gameEvent.type == sf::Event::TextEntered)
                {

                    if (gameEvent.text.unicode == 8)
                    {
                        if (!playerNameEnter.empty())
                            playerNameEnter.pop_back();
                    }
                    else if (gameEvent.text.unicode == 32 && playerNameEnter.size() < 8)
                    {
                        if (!playerNameEnter.empty())
                            playerNameEnter += char(gameEvent.text.unicode);
                    }
                    else if(playerNameEnter.size() < 8)
                        playerNameEnter += char(gameEvent.text.unicode);
                    
                }

            //Mouse control
            if (gameEvent.type == Event::MouseButtonPressed && gameEvent.mouseButton.button == Mouse::Left)
            {
                if (CheckPosition(leaderBoardButton, mouse, gameWindow) && !leaderBoard)
                {
                    leaderBoard = true;
                    leaderBoardNameText.setString(LeaderBoardText(0));
                    leaderBoardScoreText.setString(LeaderBoardText(1));
                }
                else if (CheckPosition(audioOnOffButton, mouse, gameWindow) && delayBeforClickTimer.asSeconds() >= 0.1)
                {
                    if (musicPlay)
                    {
                        mainTheme.pause();
                        musicPlay = false;
                        audioOnOffButton.setTexture(&audioOffTexture, true);
                    }
                    else if (!musicPlay)
                    {
                        mainTheme.play();
                        musicPlay = true;
                        audioOnOffButton.setTexture(&audioOnTexture, true);
                    }
                }
                else if (CheckPosition(leaderBoardButton, mouse, gameWindow) && leaderBoard)
                    leaderBoard = false;
                if (lose && !leaderBoard)
                {
                    if (CheckPosition(notSaveMyResultsButton, mouse, gameWindow))
                    {
                        Restart();
                        playerNameEnter = "";
                    }
                    else if (CheckPosition(saveMyResultsButton, mouse, gameWindow) && !playerNameEnter.empty())
                    {
                        SaveResult(playerNameEnter, score);
                        Restart();
                        playerNameEnter = "";
                    }
                }
            }
        }

        // game
        if (!pause && !lose && !leaderBoard)
        {
            deletedPerTick = 0;
            // Figure fall
            if ((dificulty >= 29 && figureMoveTimer.asSeconds() >= dificultySpeedTable[28]) || figureMoveTimer.asSeconds() >= dificultySpeedTable[dificulty])
            {
                FigureFall();
                figureMoveTime.restart();
            }
            // spawn new figure, if need
            if (newFigure)
                SpawnFigure(1);

            // Delete line
            for (yGlobalCord = 0; yGlobalCord < squareOnPosition.size(); yGlobalCord++)
            {
                existInThisSeries = 0;
                for (xGlobalCord = 0; xGlobalCord < squareOnPosition[yGlobalCord].size(); xGlobalCord++)
                {
                    if (squareOnPosition[yGlobalCord][xGlobalCord])
                        existInThisSeries++;
                }
                if (existInThisSeries == squareOnPosition[yGlobalCord].size())
                {
                    for (xGlobalCord = 0; xGlobalCord < squareOnPosition[yGlobalCord].size(); xGlobalCord++)
                    {
                        squareOnPosition[yGlobalCord][xGlobalCord] = false;
                        gameFieldVisual[yGlobalCord][xGlobalCord].setFillColor(Color::White);
                    }
                    deletedSeries = yGlobalCord;
                    clearedLines++;
                    if (clearedLines % 10 == 0)
                        dificulty++;
                    deletedPerTick++;
                    deleted = true;
                }
                if (deleted)
                {
                    gameFieldVisualBufer = gameFieldVisual;
                    squareOnPositionBufer = squareOnPosition;

                    for (yGlobalCord = 1; yGlobalCord <= deletedSeries; yGlobalCord++)
                        for (xGlobalCord = 0; xGlobalCord < gameFieldVisual[yGlobalCord].size(); xGlobalCord++)
                        {
                            gameFieldVisual[yGlobalCord][xGlobalCord].setFillColor(gameFieldVisualBufer[yGlobalCord - 1][xGlobalCord].getFillColor());
                            squareOnPosition[yGlobalCord][xGlobalCord] = squareOnPositionBufer[yGlobalCord - 1][xGlobalCord];
                        }
                    deleted = false;
                }
            }
            // add scores per deleted lines
            switch (deletedPerTick)
            {
            //1 line
            case 1:
                score += 100 * (dificulty + 1);
                break;
            //2 lines
            case 2:
                score += 400 * (dificulty + 1);
                break;
            //3 lines
            case 3:
                score += 900 * (dificulty + 1);
                break;
            //Tetris
            case 4:
                score += 2000 * (dificulty + 1);
                break;
            //not deleted in this frame
            default:
                break;
            }
            // Lose conditions
            for (xGlobalCord = 0; xGlobalCord < squareOnPosition[0].size(); xGlobalCord++)
                if (squareOnPosition[0][xGlobalCord])
                    lose = true;
        }

        //Update text
        //score
        scoreText.setString("Score \n" + to_string(score));
        //cleared
        clearedLinesText.setString("Cleared \n" + to_string(clearedLines));
        //dificulty
        levelText.setString("Level \n" + to_string(dificulty));

        // redraw
        gameWindow.clear(Color(214, 214, 214));
        if (!leaderBoard)
        {
            //gamefield
            for (yGlobalCord = 0; yGlobalCord < gameFieldVisual.size(); yGlobalCord++)
                for (xGlobalCord = 0; xGlobalCord < gameFieldVisual[yGlobalCord].size(); xGlobalCord++)
                    gameWindow.draw(gameFieldVisual[yGlobalCord][xGlobalCord]);
            //current figure
            for (yLocalCord = 0; yLocalCord < currentFigureVisual.size(); yLocalCord++)
                for (xLocalCord = 0; xLocalCord < currentFigureVisual[yLocalCord].size(); xLocalCord++)
                    if (figures[currentFigure][currentFigureRotate][yLocalCord][xLocalCord])
                        gameWindow.draw(currentFigureVisual[yLocalCord][xLocalCord]);
            //next figure
            for (yLocalCord = 0; yLocalCord < currentFigureVisual.size(); yLocalCord++)
                for (xLocalCord = 0; xLocalCord < currentFigureVisual[yLocalCord].size(); xLocalCord++)
                    if (figures[nextFigure][0][yLocalCord][xLocalCord])
                        gameWindow.draw(nextFigureVisual[yLocalCord][xLocalCord]);
            // text
            gameWindow.draw(scoreText);
            gameWindow.draw(clearedLinesText);
            gameWindow.draw(levelText);
            gameWindow.draw(nextFigureText);
            if (lose)
            {
                playerNameEnterText.setString(playerNameEnter);
                gameWindow.draw(grayFilter);
                gameWindow.draw(loseText);
                gameWindow.draw(saveMyResultsButton);
                gameWindow.draw(notSaveMyResultsButton);
                gameWindow.draw(myNameEnterField);
                gameWindow.draw(playerNameEnterText);
                gameWindow.draw(notSaveMyResultsText);
                gameWindow.draw(saveMyResultsText);
                gameWindow.draw(myNameText);
            }
            //Icons & buttons
            //Icon
            if (pause)
                gameWindow.draw(pauseIcon);
        }
        else if(leaderBoard)
        {
            gameWindow.draw(leaderBoardTile);
            gameWindow.draw(leaderBoardNameText);
            gameWindow.draw(leaderBoardScoreText);
        }
            //Button
            gameWindow.draw(leaderBoardButton);
            gameWindow.draw(audioOnOffButton);

        gameWindow.display();
    }

    return 0;
}
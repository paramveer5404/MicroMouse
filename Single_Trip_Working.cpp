#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <queue>
#include <windows.h>

#include "API.h"
#include "Cell1.h"

using namespace std;
int mazeSize = 16; // 16X16
int centerX = mazeSize / 2;
int centerY = mazeSize / 2;
/*
    * orientation-stores the direction faced by the arrow head
    *  possible orientation values:
    *       N=> North
    *       S=>South
    *       W=>West
    *       E=>East
    * 
    */
// Environment Variables
char orientation = 'N'; // current orientation

int x = 0, y = 0; // current coordinates

//---------------------function signatures--------------------
void log(const std::string &text);
void logInline(const std::string &text);
const std::string currentDateTime();
bool isGoal(int x, int y);
bool isSafe(int X, int Y);
void updateposition(char currentMove);
void exploreCell(vector<vector<Cell>> &map, int x, int y);
void initializeMaze(vector<vector<Cell>> &map);
void findGoal(vector<vector<Cell>> &map);
Cell &getFrontCell(vector<vector<Cell>> &map);
Cell &getLeftCell(vector<vector<Cell>> &map);
Cell &getRightCell(vector<vector<Cell>> &map);
void getMinDistanceDirection(vector<vector<Cell>> &map, int &minDistance, char &minDirection);
void floodOpenNeighbours(vector<vector<Cell>> &map);
void moveInDirection(vector<vector<Cell>> &map, char direction);
void setMazeValue(vector<vector<Cell>> &map){
    for (int i = 0; i < mazeSize; i++)
        for (int j = 0; j < mazeSize; j++)
         API::setText(i,j,to_string(map[i][j].getFloodFillCost()));
}

void KnownWalls(vector<vector<Cell>> &map){
    for(int i=0; i<mazeSize; i++){
        for(int j=0; j<mazeSize; j++){
            if(map[i][j].hasNorthWall()) API::setWall(i,j,'n');
            if(map[i][j].hasSouthWall()) API::setWall(i,j,'s');
            if(map[i][j].hasEastWall()) API::setWall(i,j,'e');
            if(map[i][j].hasWestWall()) API::setWall(i,j,'w');
        }
    }
}

//---------------------main()--------------
int main(int argc, char *argv[])
{

    vector<vector<Cell>> map(mazeSize, vector<Cell>(mazeSize));
    initializeMaze(map);
    setMazeValue(map);
    log("END");
    // for(int i=0; i<mazeSize; i++){
    //     for(int j=0; j<mazeSize; j++){
    //         API::clearWall(i,j,'n');
    //         API::clearWall(i,j,'s');
    //        API::clearWall(i,j,'e');
    //        API::clearWall(i,j,'w');
    //     }
    // }
    findGoal(map);
    setMazeValue(map);
}
//---------------------- function defintions-------------------------------
bool isGoal(int x, int y)
{

    return ((x == 7 || x == 8) && (y == 7 || y == 8));
}

// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime()
{
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);
    printf("The current date/time is: %s", asctime(timeinfo));
    return asctime(timeinfo);
}

void log(const std::string &text)
{
    std::cerr << text << std::endl;
}

void logInline(const std::string &text)
{
    std::cerr << text;
}
void updateposition(char currentMove)
{
    // update the orientation and coordinates
    switch (orientation)
    {
    case 'N':
        if (currentMove == 'l')
            orientation = 'W';
        else if (currentMove == 'r')
            orientation = 'E';

        else if (currentMove == 'f')
            ++y;
        break;
    case 'S':
        if (currentMove == 'l')
            orientation = 'E';
        else if (currentMove == 'r')
            orientation = 'W';
        else if (currentMove == 'f')
            --y;
        break;
    case 'E':
        if (currentMove == 'l')
            orientation = 'N';
        else if (currentMove == 'r')
            orientation = 'S';
        else if (currentMove == 'f')
            ++x;
        break;
    case 'W':
        if (currentMove == 'l')
            orientation = 'S';
        else if (currentMove == 'r')
            orientation = 'N';
        else if (currentMove == 'f')
            --x;
        break;
    default:
        x = 0, y = 0;
    }
}

void exploreCell(vector<vector<Cell>> &map, int x, int y)
{
    // map[x][y].setVisited(); // first visit is straight, then right, then left, then back
    if (map[x][y].getHasBeenExplored())
        return;

    map[x][y].sethasBeenExplored(true);

    switch (orientation)
    {
    case 'N':

        map[x][y].setNorthWall(API::wallFront());
        map[x][y].setEastWall(API::wallRight());
        map[x][y].setWestWall(API::wallLeft());
        break;
    case 'S':

        map[x][y].setSouthWall(API::wallFront());
        map[x][y].setEastWall(API::wallLeft());
        map[x][y].setWestWall(API::wallRight());
        break;
    case 'W':

        map[x][y].setWestWall(API::wallFront());
        map[x][y].setSouthWall(API::wallLeft());
        map[x][y].setNorthWall(API::wallRight());
        break;
    case 'E':

        map[x][y].setEastWall(API::wallFront());
        map[x][y].setNorthWall(API::wallLeft());
        map[x][y].setSouthWall(API::wallRight());
        break;
    default:
        int a = 0;
    }
    if(isSafe(x,y+1) && map[x][y].hasNorthWall()) map[x][y+1].setSouthWall(true);
    if(isSafe(x,y-1) && map[x][y].hasSouthWall()) map[x][y-1].setNorthWall(true);
    if(isSafe(x+1,y) && map[x][y].hasEastWall()) map[x+1][y].setWestWall(true);
    if(isSafe(x-1,y) && map[x][y].hasWestWall()) map[x-1][y].setEastWall(true);
}

void initializeMaze(vector<vector<Cell>> &map)
{

    for (int i = 0; i < mazeSize; i++)
    {
        map[i].reserve(mazeSize);
    }
    for (int i = 0; i < mazeSize; i++)
    {
        for (int j = 0; j < mazeSize; j++)
        {
            map[i][j].setFloodFillCost(-1);
        }
    }

    map[0][0].setSouthWall(true); // leads to errors
}

void findGoal(vector<vector<Cell>> &map) // takes coordinates of current cell
{
    bool destinationFound = false;
    int minDistance;
    char minDirection;
    // alias for conveneience
    while (!destinationFound)
    {

        exploreCell(map, x, y); // explore current cell
        minDistance = mazeSize * mazeSize* 2;
        // if goal found-> exit while loop
        if (isGoal(x, y))
            destinationFound = true;
        KnownWalls(map);
        getMinDistanceDirection(map, minDistance, minDirection);
        // reflood
        initializeMaze(map);
        floodOpenNeighbours(map);
        //print maze after reflooding
        setMazeValue(map);
        // after reflooding get new min distance neighbours
        getMinDistanceDirection(map, minDistance, minDirection); //get neighbour with lowest distance

        moveInDirection(map, minDirection);
    }
}

// f-move front ,b- move back,r-move right, l-moveleft
void moveInDirection(vector<vector<Cell>> &map, char direction)
{
    // move to the  neighbouring cell with the lowest distance cost
    int prevX = x, prevY = y;
    if (direction == 'f')
    {
        API::moveForward();
        updateposition('f');
    }
    else if (direction == 'l')
    {
        API::turnLeft();
        updateposition('l');
        API::moveForward();
        updateposition('f');
    }

    else if (direction == 'r')
    {

        API::turnRight();
        updateposition('r');
        API::moveForward();
        updateposition('f');
    }
    else if (direction == 'b')
    {
        API::turnRight();
        updateposition('r');
        API::turnRight();
        updateposition('r');
        API::moveForward();
        updateposition('f');
    }

    map[x][y].setPrevVisitedCell(&map[prevX][prevY]);
}
// doesnt check if coordinates are within boundaries
Cell &getFrontCell(vector<vector<Cell>> &map)
{
    switch (orientation)
    {
    case 'N': return map[x][y + 1];
    case 'S': return map[x][y - 1];
    case 'E': return map[x + 1][y];
    case 'W': return map[x - 1][y];
    }
}
Cell &getLeftCell(vector<vector<Cell>> &map) {
    switch (orientation){
    case 'N': return map[x - 1][y];
    case 'S': return map[x + 1][y];
    case 'E': return map[x][y + 1];
    case 'W': return map[x][y - 1];
    }
}
Cell &getRightCell(vector<vector<Cell>> &map){
    switch (orientation){
    case 'N': return map[x + 1][y];
    case 'S': return map[x - 1][y];
    case 'E': return map[x][y - 1];
    case 'W': return map[x][y + 1];
    }
}

//get min distance relative to current position and relative position f = front, b = behind, l=left,r=right
void getMinDistanceDirection(vector<vector<Cell>> &map, int &minDistance, char &minDirection)
{
    if (!API::wallFront())
    {
        Cell front = getFrontCell(map);

        if (minDistance > front.getFloodFillCost())
        {
            minDistance = front.getFloodFillCost();
            minDirection = 'f';
        }
    }
    if (!API::wallLeft())
    {
        Cell left = getLeftCell(map);
        if (minDistance > left.getFloodFillCost())
        {
            minDistance = left.getFloodFillCost();
            minDirection = 'l';
        }
    }
    if (!API::wallRight())
    {
        Cell right = getRightCell(map);
        if (minDistance > right.getFloodFillCost())
        {
            minDistance = right.getFloodFillCost();
            minDirection = 'r';
        }
    }
    if (map[x][y].getPrevVisitedCell() != nullptr)
    {
        Cell *back = map[x][y].getPrevVisitedCell();
        if (minDistance > back->getFloodFillCost())
        {
            minDistance = back->getFloodFillCost();
            minDirection = 'b';
        }
    }
}

void floodOpenNeighbours(std::vector<std::vector<Cell>> &map) {
    std::queue<Coordinates> floodQueue;
    initializeMaze(map);
    floodQueue.push(Coordinates{centerX, centerY});
    floodQueue.push(Coordinates{centerX-1, centerY});
    floodQueue.push(Coordinates{centerX, centerY-1});
    floodQueue.push(Coordinates{centerX-1, centerY-1});
    map[centerX][centerY].setFloodFillCost(0); // Set the goal cell's value to 0
    map[centerX-1][centerY].setFloodFillCost(0);
    map[centerX][centerY-1].setFloodFillCost(0);
    map[centerX-1][centerY-1].setFloodFillCost(0);
    while (!floodQueue.empty()) {
        int cellX = floodQueue.front().x;
        int cellY = floodQueue.front().y;
        floodQueue.pop();
        // API::setColor(cellX,cellY,'B');

        Cell &cell = map[cellX][cellY];
        int currentCost = cell.getFloodFillCost();

        // Check neighbors
        std::vector<Coordinates> neighbors;

        if (!cell.hasNorthWall() && isSafe(cellX, cellY + 1)) {
            neighbors.push_back(Coordinates{cellX, cellY + 1});
        }
        if (!cell.hasSouthWall() && isSafe(cellX, cellY - 1)) {
            neighbors.push_back(Coordinates{cellX, cellY - 1});
        }
        if (!cell.hasWestWall() && isSafe(cellX - 1, cellY)) {
            neighbors.push_back(Coordinates{cellX - 1, cellY});
        }
        if (!cell.hasEastWall() && isSafe(cellX + 1, cellY)) {
            neighbors.push_back(Coordinates{cellX + 1, cellY});
        }

        for (const auto &neighbor : neighbors) {
            Cell &neighborCell = map[neighbor.x][neighbor.y];
            if (neighborCell.getFloodFillCost() == -1 || neighborCell.getFloodFillCost()>currentCost+1) {
                neighborCell.setFloodFillCost(currentCost + 1);
                floodQueue.push(neighbor);
            }
        }
    }
}
bool isSafe(int X, int Y)
{
    if (X < 0 || X > mazeSize - 1)
        return false;
    if (Y < 0 || Y > mazeSize - 1)
        return false;
    return true;
}

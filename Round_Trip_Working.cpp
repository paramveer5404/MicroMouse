#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <queue>
#include <string>
#include "API.h"

using namespace std;
int mazeSize = API::mazeWidth(); //assuming a Square Maze
int centerX = mazeSize / 2;
int centerY = mazeSize / 2;
int startX = 0;
int startY = 0;
char orientation = 'N'; // current orientation
int x = 0, y = 0; // current coordinates
struct Coordinates{
    int x,y;
};
enum mode
{
    FIND_CENTRE,
    FIND_START
};
mode searchMode = FIND_CENTRE;
int Exploration_Rounds = 2;

//------------------Class Cell-----------------------//

class Cell
{
public:
    Cell(){
        // no walls initially
        northWall = false;
        southWall = false;
        westWall = false;
        eastWall = false;
        hasBeenExplored = false;
    }

    // setters
    void sethasBeenExplored(bool status){ hasBeenExplored = status; }
    void setNorthWall(bool wall){ northWall = wall; }
    void setSouthWall(bool wall){ southWall = wall; }
    void setWestWall(bool wall){ westWall = wall;}
    void setEastWall(bool wall){ eastWall = wall;}
    void setFloodFillCost(int cost){ floodFillCost = cost;}

    // getters
    bool getHasBeenExplored() { return hasBeenExplored; }
    bool hasNorthWall() { return northWall; }
    bool hasSouthWall() { return southWall; }
    bool hasWestWall() { return westWall; }
    bool hasEastWall() { return eastWall; }
    int getFloodFillCost() { return floodFillCost; }

private:
    bool northWall, southWall, westWall, eastWall, hasBeenExplored;
    int floodFillCost;
};


//---------------------function signatures--------------------

void log(const std::string &text);
bool isGoal(int x, int y);
bool isSafe(int X, int Y);
void updateposition(char currentMove);
void exploreCell(vector<vector<Cell>> &map, int x, int y);
void initializeMaze(vector<vector<Cell>> &map);
void findGoal(vector<vector<Cell>> &map);
void getMinDistanceDirection(vector<vector<Cell>> &map, int &minDistance, char &minDirection);
void floodOpenNeighbours(vector<vector<Cell>> &map);
void moveInDirection(vector<vector<Cell>> &map, char direction);
void setMazeValue(vector<vector<Cell>> &map);
void KnownWalls(vector<vector<Cell>> &map);

//---------------------main()--------------
int main(int argc, char *argv[])
{
    vector<vector<Cell>> map(mazeSize, vector<Cell>(mazeSize));
    log("Maze Width: " + to_string(mazeSize));
    for(int round = 1; round <= Exploration_Rounds; round++){
        searchMode = FIND_CENTRE;
        log("Round " + to_string(round) + ": Moving Towards Center");
        findGoal(map);
        searchMode = FIND_START;
        log("Round " + to_string(round) + ": Moving Towards Start");
        findGoal(map);
    }
    log("Final Run");
    searchMode = FIND_CENTRE;
    findGoal(map);
    log("Run Ended");
}
//---------------------- function defintions-------------------------------

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

bool isGoal(int x, int y)
{
    if (searchMode == FIND_CENTRE)
        return ((x == 7 || x == 8) && (y == 7 || y == 8));
    else
        return ((x == 0) && (y == 0));
}

void log(const std::string &text) { cerr << text << std::endl; }

// updates the orientation and current, x, y coordinates based on what move was made
// must be called after each move
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
    {
        return;
    }
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
        KnownWalls(map);
        minDistance = mazeSize * mazeSize* 2;
        // if goal fount-> exit while loop
        if (isGoal(x, y)){
            destinationFound = true;
            break;
        }
        floodOpenNeighbours(map);
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
}

//get min distance relative to current position and relative position f = front, b = behind, l=left,r=right
void getMinDistanceDirection(vector<vector<Cell>> &map, int &minDistance, char &minDirection)
{
    if(!map[x][y].hasNorthWall()){
        Cell cell = map[x][y+1];
        if (minDistance > cell.getFloodFillCost())
        {
            minDistance = cell.getFloodFillCost();
            if(orientation=='N') minDirection = 'f';
            if(orientation=='S') minDirection = 'b';
            if(orientation=='E') minDirection = 'l';
            if(orientation=='W') minDirection = 'r';
        }
    }
    if(!map[x][y].hasSouthWall()){
        Cell cell = map[x][y-1];
        if (minDistance > cell.getFloodFillCost())
        {
            minDistance = cell.getFloodFillCost();
            if(orientation=='N') minDirection = 'b';
            if(orientation=='S') minDirection = 'f';
            if(orientation=='E') minDirection = 'r';
            if(orientation=='W') minDirection = 'l';
        }
    }
    if(!map[x][y].hasEastWall()){
        Cell cell = map[x+1][y];
        if (minDistance > cell.getFloodFillCost())
        {
            minDistance = cell.getFloodFillCost();
            if(orientation=='N') minDirection = 'r';
            if(orientation=='S') minDirection = 'l';
            if(orientation=='E') minDirection = 'f';
            if(orientation=='W') minDirection = 'b';
        }
    }
    if(!map[x][y].hasWestWall()){
        Cell cell = map[x-1][y];
        if (minDistance > cell.getFloodFillCost())
        {
            minDistance = cell.getFloodFillCost();
            if(orientation=='N') minDirection = 'l';
            if(orientation=='S') minDirection = 'r';
            if(orientation=='E') minDirection = 'b';
            if(orientation=='W') minDirection = 'f';
        }
    }
}


void floodOpenNeighbours(std::vector<std::vector<Cell>> &map) {
    std::queue<Coordinates> floodQueue;
    initializeMaze(map);
    if(searchMode==FIND_CENTRE){

        floodQueue.push(Coordinates{centerX, centerY});
        floodQueue.push(Coordinates{centerX-1, centerY});
        floodQueue.push(Coordinates{centerX, centerY-1});
        floodQueue.push(Coordinates{centerX-1, centerY-1});
        map[centerX][centerY].setFloodFillCost(0); // Set the goal cell's value to 0
        map[centerX-1][centerY].setFloodFillCost(0);
        map[centerX][centerY-1].setFloodFillCost(0);
        map[centerX-1][centerY-1].setFloodFillCost(0);

    }
    if(searchMode==FIND_START){

        floodQueue.push(Coordinates{startX, startY});
        map[startX][startY].setFloodFillCost(0); // Set the goal cell's value to 0

    }
    while (!floodQueue.empty()) {
        int cellX = floodQueue.front().x;
        int cellY = floodQueue.front().y;
        floodQueue.pop();

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
bool isSafe(int X, int Y){
    if (X < 0 || X > mazeSize - 1) return false;
    if (Y < 0 || Y > mazeSize - 1) return false;
    return true;
}
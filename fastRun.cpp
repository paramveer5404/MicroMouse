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
struct Coordinates {
    int x, y, direction, straightCount;
};

enum mode
{
    FIND_CENTRE,
    FIND_START
};
mode searchMode = FIND_CENTRE;
int MAX = 1e9;
int Exploration_Rounds = 2;
int forwardCost = 10;
int turnCost = 200;
int groupingCost = 5;
int discountAfter = 2;

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
void getMinDistanceDirection(vector<vector<Cell>> &map, int &minDistance, char &minDirection,int x,int y, char orientation);
void floodFill(vector<vector<Cell>> &map);
void moveInDirection(vector<vector<Cell>> &map, char direction);
void setMazeValue(vector<vector<Cell>> &map);
void fastRun(vector<vector<Cell>> &safeMap,vector<vector<Cell>> &map);
void initialize_safeMap(vector<vector<Cell>> &safeMap,vector<vector<Cell>> &map);
void floodFill_Modified(vector<vector<Cell>> &map);
string find_safeRoute(vector<vector<Cell>> &safeMap);
void updateVirtualPosition(char currentMove,char &virtualOrientation,int &virtualX,int &virtualY);
void lookNorth();

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
    log("Final Speed Run");
    searchMode = FIND_CENTRE;
    vector <vector<Cell>> safeMap(mazeSize,vector<Cell>(mazeSize));
    fastRun(safeMap,map);
    log("Run Ended");
}
//---------------------- function defintions-------------------------------

void fastRun(vector<vector<Cell>> &safeMap,vector<vector<Cell>> &map){

    initialize_safeMap(safeMap,map);   //Create a map assuming all walls closed
    floodFill_Modified(safeMap);  //Find optimal safe route
    setMazeValue(safeMap);
    lookNorth(); //Making sure that the bot is facing towards North before starting Fast Run
    string moves = find_safeRoute(safeMap); 
    int N_moves = moves.size();
    log(moves);
    for(int i=0; i<N_moves; i++){
        if(moves[i]=='F') API::moveForward();
        if(moves[i]=='R') API::turnRight();
        if(moves[i]=='L') API::turnLeft();
    }
    // int pos=0,consecutiveForwards=0;
    // while(pos < N_moves){
    //     if(moves[pos]=='R'){
    //         if(consecutiveForwards!=0) API::moveForward(consecutiveForwards);
    //         consecutiveForwards=0;
    //         API::turnRight();
    //     }
    //     if(moves[pos]=='L'){
    //         if(consecutiveForwards!=0) API::moveForward(consecutiveForwards);
    //         consecutiveForwards=0;
    //         API::turnLeft();
    //     }
    //     if(moves[pos]=='F') consecutiveForwards++;
    //     pos++;
    // }
    // if(consecutiveForwards!=0) if(consecutiveForwards!=0) API::moveForward(consecutiveForwards);

}


void initialize_safeMap(vector<vector<Cell>> &safeMap,vector<vector<Cell>> &map){
    for(int i=0; i<mazeSize; i++){
        for(int j=0; j<mazeSize; j++){
            safeMap[i][j].sethasBeenExplored(map[i][j].getHasBeenExplored());
            safeMap[i][j].setNorthWall(true);
            safeMap[i][j].setSouthWall(true);
            safeMap[i][j].setEastWall(true);
            safeMap[i][j].setWestWall(true);

            if(map[i][j].getHasBeenExplored()==true){
                API::setColor(i,j,'B');
                safeMap[i][j].setNorthWall(map[i][j].hasNorthWall());
                safeMap[i][j].setSouthWall(map[i][j].hasSouthWall());
                safeMap[i][j].setEastWall(map[i][j].hasEastWall());
                safeMap[i][j].setWestWall(map[i][j].hasWestWall());
            }
        }
    }

    for(int i=0; i<mazeSize; i++){
        for(int j=0; j<mazeSize; j++){
            //Specifying that if current cell has no wall ahead then the ahead cell has no wall behind
            if(isSafe(i,j+1) && !safeMap[i][j].hasNorthWall()) safeMap[i][j+1].setSouthWall(false);
            if(isSafe(i,j-1) && !safeMap[i][j].hasSouthWall()) safeMap[i][j-1].setNorthWall(false);
            if(isSafe(i+1,j) && !safeMap[i][j].hasEastWall()) safeMap[i+1][j].setWestWall(false);
            if(isSafe(i-1,j) && !safeMap[i][j].hasWestWall()) safeMap[i-1][j].setEastWall(false);
        }
    }
    for(int i=0; i<mazeSize; i++){
        for(int j=0; j<mazeSize; j++){
            //Displaying safe map on simulator
            API::clearWall(i,j,'n');
            API::clearWall(i,j,'s');
            API::clearWall(i,j,'e');
            API::clearWall(i,j,'w');

            if(safeMap[i][j].hasNorthWall()) API::setWall(i,j,'n');
            if(safeMap[i][j].hasSouthWall()) API::setWall(i,j,'s');
            if(safeMap[i][j].hasEastWall()) API::setWall(i,j,'e');
            if(safeMap[i][j].hasWestWall()) API::setWall(i,j,'w');
        }
    }
}

void floodFill_Modified(vector<vector<Cell>> &map){
    std::queue<Coordinates> floodQueue;
    initializeMaze(map);
    if (searchMode == FIND_CENTRE) {
        floodQueue.push(Coordinates{centerX, centerY, -1, 0});
        floodQueue.push(Coordinates{centerX - 1, centerY, -1, 0});
        floodQueue.push(Coordinates{centerX, centerY - 1, -1, 0});
        floodQueue.push(Coordinates{centerX - 1, centerY - 1, -1, 0});
        map[centerX][centerY].setFloodFillCost(0);
        map[centerX - 1][centerY].setFloodFillCost(0);
        map[centerX][centerY - 1].setFloodFillCost(0);
        map[centerX - 1][centerY - 1].setFloodFillCost(0);
    }
    if (searchMode == FIND_START) {
        floodQueue.push(Coordinates{startX, startY, -1, 0});
        map[startX][startY].setFloodFillCost(0);
    }
    
    while (!floodQueue.empty()) {
        int cellX = floodQueue.front().x;
        int cellY = floodQueue.front().y;
        int direction = floodQueue.front().direction;
        int straightCount = floodQueue.front().straightCount;
        floodQueue.pop();

        Cell &cell = map[cellX][cellY];
        int currentCost = cell.getFloodFillCost();

        std::vector<Coordinates> neighbors;
        

        //Giving Low Weightage to path with less turns
        if (!cell.hasNorthWall() && isSafe(cellX, cellY + 1)) {
            neighbors.push_back(Coordinates{cellX, cellY + 1, 0, (direction == 0) ? straightCount + 1 : 1});
        }
        if (!cell.hasSouthWall() && isSafe(cellX, cellY - 1)) {
            neighbors.push_back(Coordinates{cellX, cellY - 1, 2, (direction == 2) ? straightCount + 1 : 1});
        }
        if (!cell.hasWestWall() && isSafe(cellX - 1, cellY)) {
            neighbors.push_back(Coordinates{cellX - 1, cellY, 3, (direction == 3) ? straightCount + 1 : 1});
        }
        if (!cell.hasEastWall() && isSafe(cellX + 1, cellY)) {
            neighbors.push_back(Coordinates{cellX + 1, cellY, 1, (direction == 1) ? straightCount + 1 : 1});
        }

        //Giving Low weightage to Path with higher straights
        for (const auto &neighbor : neighbors) {
            Cell &neighborCell = map[neighbor.x][neighbor.y];

            // Apply discount for consecutive straight moves
            int distanceCost = (neighbor.straightCount > discountAfter)? groupingCost : forwardCost;
            int turnPrice = (direction != -1 && direction != neighbor.direction) ? turnCost : 0;

            int newCost = currentCost + distanceCost + turnPrice;

            if (neighborCell.getFloodFillCost() == -1 || neighborCell.getFloodFillCost() > newCost) {
                neighborCell.setFloodFillCost(newCost);
                floodQueue.push(neighbor);
            }
        }
    }
}



string find_safeRoute(vector<vector<Cell>> &safeMap){
    string moves;
    int virtualX=0,virtualY=0;
    char virtualOrientation = 'N';

    while(!isGoal(virtualX,virtualY)){
        int minDistance=1e5;
        char minDirection;
        getMinDistanceDirection(safeMap,minDistance,minDirection,virtualX,virtualY,virtualOrientation);
        if(minDirection=='r'){
            moves.push_back('R');
            updateVirtualPosition('r',virtualOrientation,virtualX,virtualY);
        }
        if(minDirection=='l') {
            moves.push_back('L');
            updateVirtualPosition('l',virtualOrientation,virtualX,virtualY);
        }
        updateVirtualPosition('f',virtualOrientation,virtualX,virtualY);
        moves.push_back('F');
    }
    return moves;
}

void updateVirtualPosition(char currentMove,char &virtualOrientation,int &virtualX,int &virtualY){
    // update the orientation and coordinates
    switch (virtualOrientation){
    case 'N':
        if (currentMove == 'l') virtualOrientation = 'W';
        else if (currentMove == 'r') virtualOrientation = 'E';
        else if (currentMove == 'f') virtualY++;
        break;
    case 'S':
        if (currentMove == 'l') virtualOrientation = 'E';
        else if (currentMove == 'r') virtualOrientation = 'W';
        else if (currentMove == 'f') virtualY--;
        break;
    case 'E':
        if (currentMove == 'l') virtualOrientation = 'N';
        else if (currentMove == 'r') virtualOrientation = 'S';
        else if (currentMove == 'f') virtualX++;
        break;
    case 'W':
        if (currentMove == 'l') virtualOrientation = 'S';
        else if (currentMove == 'r') virtualOrientation = 'N';
        else if (currentMove == 'f') virtualX--;
        break;
    default: log(currentMove+" - Not a Valid Orientation.");
    }
}

void lookNorth(){
    if(orientation=='S') {
        API::turnRight();
        updateposition('r');
        API::turnRight();
        updateposition('r');
    }
    else if(orientation=='W'){
        API::turnRight();
        updateposition('r');
    }
    else if(orientation=='E'){
        API::turnLeft();
        updateposition('l');
    }
}



void setMazeValue(vector<vector<Cell>> &map){
    for (int i = 0; i < mazeSize; i++)
        for (int j = 0; j < mazeSize; j++)
         API::setText(i,j,to_string(map[i][j].getFloodFillCost()));
}

bool isGoal(int x, int y){
    if (searchMode == FIND_CENTRE)
        return ((x == 7 || x == 8) && (y == 7 || y == 8));
    else
        return ((x == 0) && (y == 0));
}

void log(const std::string &text) { cerr << text << std::endl; }


// updates the orientation and current, x, y coordinates based on what move was made
// must be called after each move

void updateposition(char currentMove){
    // update the orientation and coordinates
    switch (orientation) {
    case 'N':
        if (currentMove == 'l') orientation = 'W';
        else if (currentMove == 'r') orientation = 'E';
        else if (currentMove == 'f') y++;
        break;
    case 'S':
        if (currentMove == 'l') orientation = 'E';
        else if (currentMove == 'r') orientation = 'W';
        else if (currentMove == 'f') y--;
        break;
    case 'E':
        if (currentMove == 'l') orientation = 'N';
        else if (currentMove == 'r') orientation = 'S';
        else if (currentMove == 'f') x++;
        break;
    case 'W':
        if (currentMove == 'l') orientation = 'S';
        else if (currentMove == 'r') orientation = 'N';
        else if (currentMove == 'f') x--;
        break;
    default: log(currentMove+" - Not a Valid Orientation.");
    }
}

void exploreCell(vector<vector<Cell>> &map, int x, int y){
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
    //Specifying that if current cell has a wall ahead then the ahead cell has a wall behind
    if(isSafe(x,y+1) && map[x][y].hasNorthWall()) map[x][y+1].setSouthWall(true);
    if(isSafe(x,y-1) && map[x][y].hasSouthWall()) map[x][y-1].setNorthWall(true);
    if(isSafe(x+1,y) && map[x][y].hasEastWall()) map[x+1][y].setWestWall(true);
    if(isSafe(x-1,y) && map[x][y].hasWestWall()) map[x-1][y].setEastWall(true);

    //Setting the wall as visited on the Maze
    if(map[x][y].hasNorthWall()) API::setWall(x,y,'n');
    if(map[x][y].hasSouthWall()) API::setWall(x,y,'s');
    if(map[x][y].hasEastWall()) API::setWall(x,y,'e');
    if(map[x][y].hasWestWall()) API::setWall(x,y,'w');
}

void initializeMaze(vector<vector<Cell>> &map){
    for (int i = 0; i < mazeSize; i++){
        for (int j = 0; j < mazeSize; j++){
            map[i][j].setFloodFillCost(-1);
        }
    }
    map[0][0].setSouthWall(true); // leads to errors
    API::setWall(0,0,'s');
}

// takes coordinates of current cell
void findGoal(vector<vector<Cell>> &map){
    bool destinationFound = false;
    int minDistance;
    char minDirection;
    // alias for conveneience
    while (!destinationFound)
    {
        exploreCell(map, x, y); // explore current cell
        minDistance = MAX;
        // if goal fount-> exit while loop
        if (isGoal(x, y)){
            destinationFound = true;
            break;
        }
        floodFill(map);
        setMazeValue(map);
        // after reflooding get new min distance neighbours
        getMinDistanceDirection(map, minDistance, minDirection,x,y,orientation); //get neighbour with lowest distance
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
void getMinDistanceDirection(vector<vector<Cell>> &map, int &minDistance, char &minDirection,int x,int y, char orientation)
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


void floodFill(std::vector<std::vector<Cell>> &map) {
    std::queue<Coordinates> floodQueue;
    initializeMaze(map);
    if(searchMode==FIND_CENTRE){

        floodQueue.push(Coordinates{centerX, centerY,0,0});
        floodQueue.push(Coordinates{centerX-1, centerY,0,0});
        floodQueue.push(Coordinates{centerX, centerY-1,0,0});
        floodQueue.push(Coordinates{centerX-1, centerY-1,0,0});
        map[centerX][centerY].setFloodFillCost(0); // Set the goal cell's value to 0
        map[centerX-1][centerY].setFloodFillCost(0);
        map[centerX][centerY-1].setFloodFillCost(0);
        map[centerX-1][centerY-1].setFloodFillCost(0);

    }
    if(searchMode==FIND_START){

        floodQueue.push(Coordinates{startX, startY,0,0});
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
            neighbors.push_back(Coordinates{cellX, cellY + 1,0,0});
        }
        if (!cell.hasSouthWall() && isSafe(cellX, cellY - 1)) {
            neighbors.push_back(Coordinates{cellX, cellY - 1,0,0});
        }
        if (!cell.hasWestWall() && isSafe(cellX - 1, cellY)) {
            neighbors.push_back(Coordinates{cellX - 1, cellY,0,0});
        }
        if (!cell.hasEastWall() && isSafe(cellX + 1, cellY)) {
            neighbors.push_back(Coordinates{cellX + 1, cellY,0,0});
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
#include "Headers/Profile.h"
#include "Headers/IR_Sensors.h"
#include "Headers/Int_Vector.h"
#include "Headers/Maze_Algorithm.h"
#include "Headers/Motion_Planner.h"
#include "Headers/Queue.h"
#include "Headers/Cell.h"


bool isSafe(int X, int Y) {
  if (X < 0 || X > MAZE_SIZE - 1) return false;
  if (Y < 0 || Y > MAZE_SIZE - 1) return false;
  return true;
}


void updateWalls(){
  //if (maze[x][y].getHasBeenExplored()) return;
  maze[x][y].sethasBeenExplored(true);


  bool rightWallPresent = rightValues.analyzeWallPresence(Right_Threshold);
  bool leftWallPresent = leftValues.analyzeWallPresence(Left_Threshold);
  bool frontWallPresent = wallFront();
  
  switch (orientation) {
    case 'N':
      maze[x][y].setNorthWall(frontWallPresent);
      maze[x][y].setEastWall(rightWallPresent);
      maze[x][y].setWestWall(leftWallPresent);
      break;
    case 'S':
      maze[x][y].setSouthWall(frontWallPresent);
      maze[x][y].setEastWall(leftWallPresent);
      maze[x][y].setWestWall(rightWallPresent);
      break;
    case 'W':
      maze[x][y].setWestWall(frontWallPresent);
      maze[x][y].setSouthWall(leftWallPresent);
      maze[x][y].setNorthWall(rightWallPresent);
      break;
    case 'E':
      maze[x][y].setEastWall(frontWallPresent);
      maze[x][y].setNorthWall(leftWallPresent);
      maze[x][y].setSouthWall(rightWallPresent);
      break;
  }

  if (isSafe(x, y + 1) && maze[x][y].hasNorthWall()) maze[x][y + 1].setSouthWall(true);
  if (isSafe(x, y - 1) && maze[x][y].hasSouthWall()) maze[x][y - 1].setNorthWall(true);
  if (isSafe(x + 1, y) && maze[x][y].hasEastWall()) maze[x + 1][y].setWestWall(true);
  if (isSafe(x - 1, y) && maze[x][y].hasWestWall()) maze[x - 1][y].setEastWall(true);
}


void clearMazeWalls(){
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    
    for (int i = 0; i < MAZE_SIZE; i++) {
    for (int j = 0; j < MAZE_SIZE; j++) {
      maze[i][j].setNorthWall(false);
      maze[i][j].setSouthWall(false);
      maze[i][j].setEastWall(false);
      maze[i][j].setWestWall(false);
      
    }
  }
}


void initializeMaze() {
  for (int i = 0; i < MAZE_SIZE; i++) {
    for (int j = 0; j < MAZE_SIZE; j++) {
      maze[i][j].setFloodFillCost(-1);
    }
  }
}

void findGoal() {
  bool destinationFound = false;
  int minDistance;
  char minDirection;
  initializeMaze();

  while (!destinationFound) {
    // printFloodFillCost();
    minDistance = MAZE_SIZE * MAZE_SIZE * 2;
    if (isGoal(x, y)) {
      destinationFound = true;
      break;
    }
    floodOpenNeighbours();
    getMinDistanceDirection(minDistance, minDirection);
    // Serial.println(minDirection);
    moveInDirection(minDirection);
    updateWalls();

    // exploreCell(x, y);
    // Serial.println("Hi");
    delay(waitAfterMoving);
  }

  Serial.println("Found Goal");
}

bool isGoal(int x, int y) {
    if (searchMode == FIND_CENTRE)
        return ((x == MAZE_SIZE / 2 || x == MAZE_SIZE / 2 - 1) && (y == MAZE_SIZE / 2 || y == MAZE_SIZE / 2 - 1));
    else
        return ((x == startX) && (y == startY));
}

void updateposition(char currentMove) {
  switch (orientation) {
    case 'N':
      if (currentMove == 'l') orientation = 'W';
      else if (currentMove == 'r') orientation = 'E';
      else if (currentMove == 'f') ++y;
      break;
    case 'S':
      if (currentMove == 'l') orientation = 'E';
      else if (currentMove == 'r') orientation = 'W';
      else if (currentMove == 'f') --y;
      break;
    case 'E':
      if (currentMove == 'l') orientation = 'N';
      else if (currentMove == 'r') orientation = 'S';
      else if (currentMove == 'f') ++x;
      break;
    case 'W':
      if (currentMove == 'l') orientation = 'S';
      else if (currentMove == 'r') orientation = 'N';
      else if (currentMove == 'f') --x;
      break;
    default:
      x = 0, y = 0;
  }
}

void exploreCell(int x, int y) {
  //if (maze[x][y].getHasBeenExplored()) return;
  maze[x][y].sethasBeenExplored(true);

  switch (orientation) {
    case 'N':
      maze[x][y].setNorthWall(wallFront());
      // maze[x][y].setEastWall(wallRight());
      // maze[x][y].setWestWall(wallLeft());
      break;
    case 'S':
      maze[x][y].setSouthWall(wallFront());
      // maze[x][y].setEastWall(wallLeft());
      // maze[x][y].setWestWall(wallRight());
      break;
    case 'W':
      maze[x][y].setWestWall(wallFront());
      // maze[x][y].setSouthWall(wallLeft());
      // maze[x][y].setNorthWall(wallRight());
      break;
    case 'E':
      maze[x][y].setEastWall(wallFront());
      // maze[x][y].setNorthWall(wallLeft());
      // maze[x][y].setSouthWall(wallRight());
      break;
  }

  if (isSafe(x, y + 1) && maze[x][y].hasNorthWall()) maze[x][y + 1].setSouthWall(true);
  if (isSafe(x, y - 1) && maze[x][y].hasSouthWall()) maze[x][y - 1].setNorthWall(true);
  if (isSafe(x + 1, y) && maze[x][y].hasEastWall()) maze[x + 1][y].setWestWall(true);
  if (isSafe(x - 1, y) && maze[x][y].hasWestWall()) maze[x - 1][y].setEastWall(true);
}

void printFloodFillCost(){
  int i,j;
  Serial.println(" ");
  Serial.println(" ");
  for(i=0; i<MAZE_SIZE; i++){
    for(j=0; j<MAZE_SIZE; j++){
      Serial.print(maze[i][j].getFloodFillCost());
      Serial.print(" ");
    }
    Serial.println(" ");
    Serial.println(" ");
  }
}

void floodOpenNeighbours() {
  initializeMaze();
  Queue floodQueue(MAZE_SIZE*MAZE_SIZE);
    if(searchMode==0){
      floodQueue.push(Coordinates{centerX, centerY});
      floodQueue.push(Coordinates{centerX-1, centerY});
      floodQueue.push(Coordinates{centerX, centerY-1});
      floodQueue.push(Coordinates{centerX-1, centerY-1});
      maze[centerX][centerY].setFloodFillCost(0); // Set the goal cell's value to 0
      maze[centerX-1][centerY].setFloodFillCost(0);
      maze[centerX][centerY-1].setFloodFillCost(0);
      maze[centerX-1][centerY-1].setFloodFillCost(0);

  }
  if(searchMode==1){
      floodQueue.push(Coordinates{startX, startY});
      maze[startX][startY].setFloodFillCost(0); // Set the goal cell's value to 0

  }

  while (!floodQueue.isEmpty()) {
    int cellX = floodQueue.frontItem().x;
    int cellY = floodQueue.frontItem().y;
    floodQueue.pop();

    Cell &cell = maze[cellX][cellY];
    int currentCost = cell.getFloodFillCost();

    Coordinates neighbors[4];
    int neighborCount = 0;

    if (!cell.hasNorthWall() && isSafe(cellX, cellY + 1)) {
      neighbors[neighborCount++] = { cellX, cellY + 1 };
    }
    if (!cell.hasSouthWall() && isSafe(cellX, cellY - 1)) {
      neighbors[neighborCount++] = { cellX, cellY - 1 };
    }
    if (!cell.hasWestWall() && isSafe(cellX - 1, cellY)) {
      neighbors[neighborCount++] = { cellX - 1, cellY };
    }
    if (!cell.hasEastWall() && isSafe(cellX + 1, cellY)) {
      neighbors[neighborCount++] = { cellX + 1, cellY };
    }

    for (int i = 0; i < neighborCount; i++) {
      Coordinates neighbor = neighbors[i];
      Cell &neighborCell = maze[neighbor.x][neighbor.y];
      if (neighborCell.getFloodFillCost() == -1 || neighborCell.getFloodFillCost() > currentCost + 1) {
        neighborCell.setFloodFillCost(currentCost + 1);
        floodQueue.push(neighbor);
      }
    }
  }
}




void getMinDistanceDirection(int &minDistance, char &minDirection) {
  if (!maze[x][y].hasNorthWall()) {
    Cell cell = maze[x][y + 1];
    if (minDistance > cell.getFloodFillCost()) {
      minDistance = cell.getFloodFillCost();
      if (orientation == 'N') minDirection = 'f';
      if (orientation == 'S') minDirection = 'b';
      if (orientation == 'E') minDirection = 'l';
      if (orientation == 'W') minDirection = 'r';
    }
  }
  if (!maze[x][y].hasSouthWall()) {
    Cell cell = maze[x][y - 1];
    if (minDistance > cell.getFloodFillCost()) {
      minDistance = cell.getFloodFillCost();
      if (orientation == 'N') minDirection = 'b';
      if (orientation == 'S') minDirection = 'f';
      if (orientation == 'E') minDirection = 'r';
      if (orientation == 'W') minDirection = 'l';
    }
  }
  if (!maze[x][y].hasEastWall()) {
    Cell cell = maze[x + 1][y];
    if (minDistance > cell.getFloodFillCost()) {
      minDistance = cell.getFloodFillCost();
      if (orientation == 'N') minDirection = 'r';
      if (orientation == 'S') minDirection = 'l';
      if (orientation == 'E') minDirection = 'f';
      if (orientation == 'W') minDirection = 'b';
    }
  }
  if (!maze[x][y].hasWestWall()) {
    Cell cell = maze[x - 1][y];
    if (minDistance > cell.getFloodFillCost()) {
      minDistance = cell.getFloodFillCost();
      if (orientation == 'N') minDirection = 'l';
      if (orientation == 'S') minDirection = 'r';
      if (orientation == 'E') minDirection = 'b';
      if (orientation == 'W') minDirection = 'f';
    }
  }
}

void moveInDirection(char direction) {
  if (direction == 'f') {
    updateposition('f');
    moveForward();
  } else if (direction == 'l') {
    updateposition('l');
    turnLeft();
    updateposition('f');
    moveForward();
  } else if (direction == 'r') {
    updateposition('r');
    turnRight();
    updateposition('f');
    moveForward();
  } else if (direction == 'b') {
    updateposition('r');
    turnRight();
    updateposition('r');
    turnRight();
    updateposition('f');
    moveForward();
  }
}
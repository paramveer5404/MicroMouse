#ifndef MAZE_ALGORITHM_H
#define MAZE_ALGORITHM_H

#include <Arduino.h>



// ---------- Wall and Maze Functions ----------
void updateWalls();
void clearMazeWalls();
void initializeMaze();

// ---------- Flood Fill Algorithm ----------
void floodOpenNeighbours();

// ---------- Exploration and Goal Search ----------
void findGoal();
void exploreCell(int x, int y);
bool isGoal(int x, int y);

// ---------- Utility and Navigation ----------
bool isSafe(int X, int Y);
void updateposition(char currentMove);
void moveInDirection(char direction);
void printFloodFillCost();
void getMinDistanceDirection(int &minDistance, char &minDirection);

#endif  // MAZE_ALGORITHM_H

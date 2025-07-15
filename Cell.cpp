#include "Headers/Cell.h"


// Constructor
Cell::Cell() {
  northWall = false;
  southWall = false;
  westWall = false;
  eastWall = false;
  hasBeenExplored = false;
  floodFillCost = -1;
}

// Setters
void Cell::sethasBeenExplored(bool status) {
  hasBeenExplored = status;
}

void Cell::setNorthWall(bool wall) {
  northWall = wall;
}

void Cell::setSouthWall(bool wall) {
  southWall = wall;
}

void Cell::setWestWall(bool wall) {
  westWall = wall;
}

void Cell::setEastWall(bool wall) {
  eastWall = wall;
}

void Cell::setFloodFillCost(int cost) {
  floodFillCost = cost;
}

// Getters
bool Cell::getHasBeenExplored() {
  return hasBeenExplored;
}

bool Cell::hasNorthWall() {
  return northWall;
}

bool Cell::hasSouthWall() {
  return southWall;
}

bool Cell::hasWestWall() {
  return westWall;
}

bool Cell::hasEastWall() {
  return eastWall;
}

int Cell::getFloodFillCost() {
  return floodFillCost;
}

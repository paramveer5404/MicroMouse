#ifndef CELL_H
#define CELL_H

struct Coordinates {
    int x, y;
};

class Cell {
  public:
    Cell();  // Constructor

    // Setters
    void sethasBeenExplored(bool status);
    void setNorthWall(bool wall);
    void setSouthWall(bool wall);
    void setWestWall(bool wall);
    void setEastWall(bool wall);
    void setFloodFillCost(int cost);

    // Getters
    bool getHasBeenExplored();
    bool hasNorthWall();
    bool hasSouthWall();
    bool hasWestWall();
    bool hasEastWall();
    int getFloodFillCost();

  private:
    bool northWall, southWall, westWall, eastWall, hasBeenExplored;
    int floodFillCost;
};

#endif  // CELL_H

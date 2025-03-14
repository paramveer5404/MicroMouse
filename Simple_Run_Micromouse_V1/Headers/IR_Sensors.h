#ifndef IR_SENSORS_H
#define IR_SENSORS_H

#include <Arduino.h>

// ----------- Function Declarations -----------
bool wallFront();
int WallError(int leftSensorValue, int rightSensorValue, int type);
int minError(int e1, int e2, int e3);
int getDist(int pin);

// ----------- Extern Lookup Table Declaration -----------

extern int lookupTable[431];  // Table defined in .cpp

#endif  // IR_SENSORS_H

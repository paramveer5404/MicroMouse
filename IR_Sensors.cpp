#include "Headers/IR_Sensors.h"
#include "Headers/Profile.h"


// ----------- Function Definitions -----------

bool wallFront() {
    int reading = getDist(sensorFrontPin);
    return reading < Front_Threshold;
}


int WallError(int leftSensorValue, int rightSensorValue, int type) {
    bool left_wall = (leftSensorValue < Left_Threshold);
    bool right_wall = (rightSensorValue < Right_Threshold);

    if (!left_wall && !right_wall) return 0;

    if (left_wall && right_wall && type == 0)
        return rightSensorValue - leftSensorValue;

    if (left_wall && !right_wall && type == 1)
        return (leftSensorValue - wallSensorInitialLeft) * (-2);

    if (right_wall && !left_wall)
        return (rightSensorValue - wallSensorInitialRight) * 2;

    return 0;
}

int minError(int e1, int e2, int e3) {
    int abs_e1 = abs(e1);
    int abs_e2 = abs(e2);
    int abs_e3 = abs(e3);

    int smallest = min(min(abs_e1, abs_e2), abs_e3);

    if (smallest == abs_e1) return e1;
    if (smallest == abs_e2) return e2;
    return e3;
}

int getDist(int pin) {
    int analogValue = analogRead(pin);
    analogValue = constrain(analogValue, 70, 500);  // Clamp to sensor range
    return lookupTable[analogValue - 70];  // Lookup distance
}



// ----------- Lookup Table Definition -----------
int lookupTable[431] = {
    190, 186, 183, 180, 177, 174, 171, 168, 166, 163, 160, 158, 155, 153, 151,
    148, 146, 144, 142, 140, 138, 136, 134, 132, 131, 129, 127, 126, 124, 122,
    121, 119, 118, 116, 115, 114, 112, 111, 110, 108, 107, 106, 105, 104, 102,
    101, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 88, 87, 86, 85,
    84, 83, 83, 82, 81, 80, 80, 79, 78, 77, 77, 76, 75, 75, 74, 74, 73, 72, 72,
    71, 70, 70, 69, 69, 68, 68, 67, 67, 66, 66, 65, 65, 64, 64, 63, 63, 62, 62,
    61, 61, 60, 60, 59, 59, 59, 58, 58, 57, 57, 57, 56, 56, 55, 55, 55, 54, 54,
    54, 53, 53, 52, 52, 52, 51, 51, 51, 50, 50, 50, 50, 49, 49, 49, 48, 48, 48,
    47, 47, 47, 47, 46, 46, 46, 45, 45, 45, 45, 44, 44, 44, 44, 43, 43, 43, 43,
    42, 42, 42, 42, 41, 41, 41, 41, 41, 40, 40, 40, 40, 40, 39, 39, 39, 39, 39,
    38, 38, 38, 38, 38, 37, 37, 37, 37, 37, 36, 36, 36, 36, 36, 36, 35, 35, 35,
    35, 35, 35, 34, 34, 34, 34, 34, 34, 33, 33, 33, 33, 33, 33, 32, 32, 32, 32,
    32, 32, 32, 31, 31, 31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 30, 30, 29,
    29, 29, 29, 29, 29, 29, 29, 29, 28, 28, 28, 28, 28, 28, 28, 28, 27, 27, 27,
    27, 27, 27, 27, 27, 27, 27, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 25, 25,
    25, 25, 25, 25, 25, 25, 25, 25, 25, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
    24, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 22, 22, 22, 22, 22, 22,
    22, 22, 22, 22, 22, 22, 22, 22, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
    21, 21, 21, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
    19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 18,
    18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 17,
    17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
    16, 16, 16, 16, 16, 15, 15};


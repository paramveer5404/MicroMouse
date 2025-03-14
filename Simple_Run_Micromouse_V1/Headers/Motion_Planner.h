#ifndef MOTION_PLANNER_H
#define MOTION_PLANNER_H

#include <Arduino.h>



// ----------- ISR For Encoders -----------
void handleEncoderA();
void handleEncoderB();

// ----------- Wrapper Movement Functions -----------
void moveForward();
void turnLeft();
void turnRight();

// ----------- Physical Motion Control Functions -----------
void moveForward_gyro(int goal_enc, float tolerated_error, float Kp1, float Kd1, float Kp2, float Kd2, int max_speed);
void turnLeft_gyro(int goal_enc, int tolerated_error, float Kp, float Kd, int max_speed);

#endif  // MOTION_PLANNER_H

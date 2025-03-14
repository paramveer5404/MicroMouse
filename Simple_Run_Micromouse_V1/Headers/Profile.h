#ifndef PROFILE_H
#define PROFILE_H

#define MAZE_SIZE 8

#include <Arduino.h>
#include "Cell.h"
#include <MPU6050_light.h>

// ------------ Maze Matrix ------------------
extern Cell maze[MAZE_SIZE][MAZE_SIZE];


// ------------ MPU Object ------------------
extern MPU6050 mpu;


// ------------ Physical Parameters ------------------
extern float cellSize;
extern float encodersPerCm;
extern int encoderPerCell;

// ------------ Sensor Thresholds -------------------
extern int wallSensorInitialRight;
extern int wallSensorInitialLeft;
extern int Left_Threshold;
extern int Right_Threshold;
extern int Front_Threshold;

// ------------ Sensor Pins ------------------------
extern const byte sensorRightPin;
extern const byte sensorLeftPin;
extern const byte sensorFrontPin;

// ------------ Control Pins ------------------------
extern const int buttonPin;

// ------------ Maze Variables ----------------------
extern int centerX;
extern int centerY;
extern int startX;
extern int startY;
extern char orientation;
extern int x, y;
extern int waitAfterMoving;

// ------------ Mode Enum ---------------------------
enum mode {
    FIND_CENTRE,
    FIND_START
};
extern mode searchMode;
extern int Exploration_Rounds;

// ------------ Motor Driver Pins -------------------
extern const int pwmA;
extern const int pwmB;
extern const int ain1;
extern const int ain2;
extern const int bin1;
extern const int bin2;
extern const int stdby;

// ------------ Encoder Pins ------------------------
extern const int enc1A;
extern const int enc2A;
extern const int enc1B;
extern const int enc2B;

// ------------ Encoder Variables -------------------
extern volatile long encoderCountA;
extern volatile long encoderCountB;

#endif // PROFILE_H

#include "Headers/Profile.h"


MPU6050 mpu(Wire);   // Use the default Wire object

// ------------ Maze Matrix ------------------
Cell maze[MAZE_SIZE][MAZE_SIZE];

// ------------ Physical Parameters ------------------
float cellSize = 15;
float encodersPerCm = 75;
int encoderPerCell = cellSize * encodersPerCm;

// ------------ Sensor Thresholds -------------------
int wallSensorInitialRight = 26;
int wallSensorInitialLeft = 26;
int Left_Threshold = 50;
int Right_Threshold = 50;
int Front_Threshold = 70;

// ------------ Sensor Pins ------------------------
const byte sensorRightPin = 26;
const byte sensorLeftPin = 28;
const byte sensorFrontPin = 27;

// ------------ Control Pins ------------------------
const int buttonPin = 15;

// ------------ Maze Variables ----------------------
int centerX = MAZE_SIZE / 2;
int centerY = MAZE_SIZE / 2;
int startX = 0;
int startY = 0;
char orientation = 'N';
int x = 0, y = 0;
int waitAfterMoving = 200;


// ------------ Mode Enum ---------------------------
mode searchMode = FIND_CENTRE;
int Exploration_Rounds = 2;

// ------------ Motor Driver Pins -------------------
const int pwmA = 4;
const int pwmB = 10;
const int ain1 = 6;
const int ain2 = 5;
const int bin1 = 9;
const int bin2 = 8;
const int stdby = 7;

// ------------ Encoder Pins ------------------------
const int enc1A = 2;
const int enc2A = 3;
const int enc1B = 14;
const int enc2B = 11;

// ------------ Encoder Variables -------------------
volatile long encoderCountA = 0;
volatile long encoderCountB = 0;

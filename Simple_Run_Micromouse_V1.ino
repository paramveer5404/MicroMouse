#include "Headers/Profile.h"
#include "Headers/Queue.h"
#include "Headers/Cell.h"
#include "Headers/Int_Vector.h"
#include "Headers/IR_Sensors.h"
#include "Headers/Maze_Algorithm.h"
#include "Headers/Motion_Planner.h"


void setup() {
  // Start serial communication for debugging
  Serial.begin(115200);
  Wire.setSDA(0);        // Set SDA to GPIO0
  Wire.setSCL(1);        // Set SCL to GPIO1
  Wire.begin();          // Initialize Wire with specified pins
  
  mpu.begin();           // Start MPU6050 with default Wire object
  Serial.print(F("MPU6050 status: "));
  Serial.println(F("Calculating offsets, do not move MPU6050"));   
  delay(1000);
  mpu.calcGyroOffsets(); // Calibrate gyroscope
  Serial.println("Done!\n");

  // Set up encoder pins as inputs
  pinMode(enc1A, INPUT);
  pinMode(enc2A, INPUT);
  pinMode(enc1B, INPUT);
  pinMode(enc2B, INPUT);
  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(ain1, OUTPUT);
  pinMode(ain2, OUTPUT);
  pinMode(bin1, OUTPUT);
  pinMode(bin2, OUTPUT);
  pinMode(stdby, OUTPUT);
  digitalWrite(stdby, HIGH); // Enable the motor driver


  // Set up Wi-Fi LED pin as output
  pinMode(buttonPin, INPUT_PULLUP);
  // Attach an interrupt to the button pin
  attachInterrupt(digitalPinToInterrupt(buttonPin), clearMazeWalls, FALLING);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);  // Turn on LED


  // Attach interrupts for encoder pins
  attachInterrupt(digitalPinToInterrupt(enc1A), handleEncoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(enc1B), handleEncoderB, CHANGE);


  //Assumptions about the first cell
  maze[0][0].setSouthWall(true); 
  maze[0][0].setEastWall(true); 
  maze[0][0].setWestWall(true);
  maze[0][0].setNorthWall(false);
  maze[0][1].setSouthWall(false);
  maze[1][0].setWestWall(true);

  for (int round = 1; round <= Exploration_Rounds; round++) {
    searchMode = FIND_CENTRE;
    findGoal();
    searchMode = FIND_START;
    findGoal();
  }
  searchMode = FIND_CENTRE;
  findGoal();
}

void loop() {}






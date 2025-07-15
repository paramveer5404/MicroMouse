#include "Headers/Motion_Planner.h"
#include "Headers/IR_Sensors.h"  // For getDist function
#include "Headers/Profile.h"     // For thresholds and pins
#include "Headers/Int_Vector.h" //For Wall Detection


// ----------- ISR For Encoders -----------

void handleEncoderA() { 
  encoderCountA += digitalRead(enc1A) == digitalRead(enc2A) ? 1 : -1; 
}

void handleEncoderB() { 
  encoderCountB += digitalRead(enc1B) == digitalRead(enc2B) ? 1 : -1; 
}

// ----------- Wrapper Movement Functions -----------

void moveForward() {
    moveForward_gyro(30, 0.66, -1.5, -2, 4, 1.5, 200);
}

void turnLeft() {
    turnLeft_gyro(90, 2, 3, 1, 90);
}

void turnRight() {
    turnLeft_gyro(-90, 2, 3, 1, 90);  // Assuming same function for right turn with negative angle
}


// ----------- Physical Movement Functions -----------

//Goal_Enc is base speed
//Kp2 is ramp
//tolerated_error being used as ramp breakpoint
//TODO - +Kd2 being used as ratio
void moveForward_gyro(int goal_enc, float tolerated_error, float Kp1, float Kd1, float Kp2, float Kd2, int max_speed) {
  // mpu.begin();           // Start MPU6050 with default Wire object
  // mpu.calcGyroOffsets();  // Calibrate gyroscope
  mpu.update();

  encoderCountA = 0;
  encoderCountB = 0;
  int error2 = 0;
  int last_error2 = 0;  // Last error to calculate derivative
  int controlOutput2 = 0;  // PD control output for angle
  int motorPWM_A = 0;      // Motor A (left motor) speed
  int motorPWM_B = 0;      // Motor B (right motor) speed 
  float initial_angle = mpu.getAngleZ();
  float currentSpeed = goal_enc;

  rightValues.reset();
  leftValues.reset();

  // Begin PD control loop for moving forward
  while (encoderCountA+encoderCountB < 2*encoderPerCell) {

    unsigned int distanceRight = getDist(sensorRightPin);
    unsigned int distanceLeft = getDist(sensorLeftPin);

    rightValues.push_back(distanceRight);
    leftValues.push_back(distanceLeft);


    // Calculate error for angle
    int error2_IR = minError(WallError(distanceLeft,distanceRight,0),WallError(distanceLeft,distanceRight,1),WallError(distanceLeft,distanceRight,2));

    mpu.update(); 
    // Calculate error for angle
    int current_angle = mpu.getAngleZ();
    error2 = (current_angle - initial_angle) + (error2_IR)*Kd2;

    // error2 = -0.2*((float)encoderCountA-(float)encoderCountB) + (error2_IR)*Kd2;
    
    // Calculate derivative of the error
    int error_derivative2 = error2 - last_error2;

    // PD control output for angle
    controlOutput2 = Kp1 * error2 + Kd1 * error_derivative2;

    // Estimate distance and adjust motor PWM values

    if (motorPWM_A < max_speed && (encoderCountA + encoderCountB)/2 < tolerated_error*encoderPerCell) currentSpeed+=Kp2;
    else if (currentSpeed > goal_enc) currentSpeed-=Kp2;

    // Combine control outputs for motor PWM adjustments
    motorPWM_A = constrain(currentSpeed - controlOutput2, -max_speed, max_speed);  // Motor A (left motor)
    motorPWM_B = constrain(currentSpeed + controlOutput2, -max_speed, max_speed);  // Motor B (right motor)

    // Adjust Motor A (left motor) direction and speed
    if (motorPWM_A < 0) {
      digitalWrite(ain1, LOW);   // Reverse Motor A
      digitalWrite(ain2, HIGH);
      analogWrite(pwmA, abs(motorPWM_A));  // Apply PWM with absolute value
    } else {
      digitalWrite(ain1, HIGH);  // Forward Motor A
      digitalWrite(ain2, LOW);
      analogWrite(pwmA, motorPWM_A);       // Apply PWM directly
    }

    // Adjust Motor B (right motor) direction and speed
    if (motorPWM_B < 0) {
      digitalWrite(bin1, LOW);   // Reverse Motor B
      digitalWrite(bin2, HIGH);
      analogWrite(pwmB, abs(motorPWM_B));  // Apply PWM with absolute value
    } else {
      digitalWrite(bin1, HIGH);  // Forward Motor B
      digitalWrite(bin2, LOW);
      analogWrite(pwmB, motorPWM_B);       // Apply PWM directly
    }

    // Store current error as last_error for the next iteration
    last_error2 = error2;
    delay(20);
  }

  
  analogWrite(pwmA, 0);
  analogWrite(pwmB, 0);



  // sendData(serverURL_right, rightValues);
  // delay(1000); // Wait 3 seconds before sending left values
  // sendData(serverURL_left, leftValues);
  // delay(1000); // Wait before sending again
}


void turnLeft_gyro(int goal_enc, int tolerated_error, float Kp, float Kd, int max_speed) {
  encoderCountA = 0;
  encoderCountB = 0;
  // mpu.begin();           // Start MPU6050 with default Wire object
  // mpu.calcGyroOffsets();  // Calibrate gyroscope
  mpu.update(); 
  
  float initial_angle = mpu.getAngleZ();  // Capture the initial angle as offset
  float error = goal_enc;    // Initial error (target angle is 90 degrees)
  float last_error = 0;   // Last error for derivative calculation
  int controlOutput = 0;  // PD control output
  int motorPWM_A = 0;     // Motor A speed
  int motorPWM_B = 0;     // Motor B speed

  while (true) {
    mpu.update(); 
    // Calculate the error as (90 - current angle relative to initial offset)
    error = goal_enc - (mpu.getAngleZ() - initial_angle);
    float error_derivative = error - last_error;
    controlOutput = Kp * error + Kd * error_derivative;

    motorPWM_A = constrain(motorPWM_A - controlOutput, -max_speed, max_speed);
    motorPWM_B = constrain(motorPWM_B + controlOutput, -max_speed, max_speed);

    // Update motor directions and speeds for motor A
    if (motorPWM_A < 0) {
      digitalWrite(ain1, LOW);
      digitalWrite(ain2, HIGH);
      analogWrite(pwmA, abs(motorPWM_A));
    } else {
      digitalWrite(ain1, HIGH);
      digitalWrite(ain2, LOW);
      analogWrite(pwmA, motorPWM_A);
    }

    // Update motor directions and speeds for motor B
    if (motorPWM_B < 0) {
      digitalWrite(bin1, LOW);
      digitalWrite(bin2, HIGH);
      analogWrite(pwmB, abs(motorPWM_B));
    } else {
      digitalWrite(bin1, HIGH);
      digitalWrite(bin2, LOW);
      analogWrite(pwmB, motorPWM_B);
    }

    last_error = error;

    if (abs(error) <= tolerated_error) {
      break;
    }
    delay(10);
  }

  // Stop both motors after reaching the goal
  analogWrite(pwmA, 0);
  analogWrite(pwmB, 0);
}

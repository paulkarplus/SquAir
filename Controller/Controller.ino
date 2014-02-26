/* SquAir Quadcopter Arduino DUE Code
Copyright (c) 2014 Paul Karplus
This code is the controller for the SquAir quadcopter. 

*/

#include "math.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// Declare global variables
int t_last = 0;
int loop_period = 10000; // period of control loop in micros
double roll = 0, pitch = 0;
IMU imu;

#define LED_PIN 13
  
void setup() {
  Serial.begin(115200);
  // Turn on LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, true);
  
  // Initialize Serial
  Serial.begin(115200);

      
  t_last = micros();
  digitalWrite(LED_PIN, false);
}

void loop() {
  
  if (abs(micros()-t_last) >= loop_period) {
    t_last = t_last + loop_period;
    // read angle (thousandths of a degree)
    imu.getRollPitch(roll,pitch);
  }
}

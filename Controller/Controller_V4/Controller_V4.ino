/* SquAir Quadcopter Arduino DUE Code
Copyright (c) 2014 Paul Karplus
This code is the controller for the SquAir quadcopter. 

*/

#include <Servo.h>
#include "math.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>

// Declare global variables
MPU6050 accelgyro;
Adafruit_BMP085 bmp;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int alt = 0;
String commandmotor;
int selectmotor = 0;
int t_last = 0;
int loop_period = 2000; // period of control loop in micros (500hz)
int m1_cmd = 0, m2_cmd = 0, m3_cmd = 0, m4_cmd = 0;

// Define output servos
Servo servoChannel1;
Servo servoChannel2;
Servo servoChannel3;
Servo servoChannel4;

#define LED_PIN 13
#define I2C_PWR 32
// Assign your channel in pins
#define CHANNEL1_IN_PIN 2 // Throttle
#define CHANNEL2_IN_PIN 3 // Aileron
#define CHANNEL3_IN_PIN 4 // Elevator
#define CHANNEL4_IN_PIN 5 // Rudder

// Assign your channel out pins
#define CHANNEL1_OUT_PIN 10 // Front Motor
#define CHANNEL2_OUT_PIN 11 // Right Motor
#define CHANNEL3_OUT_PIN 12 // Left Motor
#define CHANNEL4_OUT_PIN 13 // Left Motor

// These bit flags are set in bUpdateFlagsShared to indicate which
// channels have new signals
#define CHANNEL1_FLAG 1
#define CHANNEL2_FLAG 2
#define CHANNEL3_FLAG 4
#define CHANNEL4_FLAG 8

// holds the update flags defined above
volatile uint32_t bUpdateFlagsShared;

// shared variables are updated by the ISR and read by loop.
// In loop we immediatley take local copies so that the ISR can keep ownership of the
// shared ones. To access these in loop
// we first turn interrupts off with noInterrupts
// we take a copy to use in loop and the turn interrupts back on
// as quickly as possible, this ensures that we are always able to receive new signals
volatile uint32_t unChannel1InShared;
volatile uint32_t unChannel2InShared;
volatile uint32_t unChannel3InShared;
volatile uint32_t unChannel4InShared;

// create local variables to hold a local copies of the channel inputs
// these are declared static so that thier values will be retained
// between calls to loop.
static int unChannel1In;
static int unChannel2In;
static int unChannel3In;
static int unChannel4In;
int ch1 = 0, ch2 = 0, ch3 = 0, ch4 = 0;

// local copy of update flags
static uint32_t bUpdateFlags;

void calcChannel1()
{
  static uint32_t ulStart;
  if(digitalRead(CHANNEL1_IN_PIN)) {
    ulStart = micros();
  } 
  else {
    unChannel1InShared = (uint32_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL1_FLAG;
  }
}

void calcChannel2()
{
  static uint32_t ulStart;
  if(digitalRead(CHANNEL2_IN_PIN)) {
    ulStart = micros();
  }
  else {
    unChannel2InShared = (uint32_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL2_FLAG;
  }
}

void calcChannel3()
{
  static uint32_t ulStart;
  if(digitalRead(CHANNEL3_IN_PIN)) {
    ulStart = micros();
  } 
  else
  {
    unChannel3InShared = (uint32_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL3_FLAG;
  }
}

void calcChannel4()
{
  static uint32_t ulStart;
  if(digitalRead(CHANNEL4_IN_PIN)) {
    ulStart = micros();
  }
  else {
    unChannel4InShared = (uint32_t)(micros() - ulStart);
    bUpdateFlagsShared |= CHANNEL4_FLAG;
  }
}

void readAngle() {
  // read raw accel/gyro measurements from device
  int t = millis();
  int t_micro = micros();
  
  // Read accelgyro data
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);  
  Serial.print(t);
  Serial.print(",");
  Serial.print(ax);
  Serial.print(",");
  Serial.print(ay);
  Serial.print(",");
  Serial.print(az);
  Serial.print(",");
  Serial.print(gx);
  Serial.print(",");
  Serial.print(gy);
  Serial.print(",");
  Serial.print(gz);
  Serial.print(",");
}

void readAlt() {
  // Read the altitude from the BMP085 pressure sensory using the Adafruit library.
  alt = bmp.readAltitude();
  /*
  Serial.print(alt);
  Serial.print(",");
  */
}

void readServos() {
  // check shared update flags to see if any channels have a new signal
  if(bUpdateFlagsShared)
  {
    noInterrupts(); // turn interrupts off quickly while we take local copies of the shared variables

    // take a local copy of which channels were updated in case we need to use this in the rest of loop
    bUpdateFlags = bUpdateFlagsShared;
    // in the current code, the shared values are always populated
    // so we could copy them without testing the flags
    // however in the future this could change, so lets
    // only copy when the flags tell us we can.
    if(bUpdateFlags & CHANNEL1_FLAG) {
      unChannel1In = unChannel1InShared;
    }
    if(bUpdateFlags & CHANNEL2_FLAG) {
      unChannel2In = unChannel2InShared;
    }
    if(bUpdateFlags & CHANNEL3_FLAG) {
      unChannel3In = unChannel3InShared;
    }
    if(bUpdateFlags & CHANNEL4_FLAG) {
      unChannel4In = unChannel4InShared;
    }
    // clear shared copy of updated flags as we have already taken the updates
    // we still have a local copy if we need to use it in bUpdateFlags
    bUpdateFlagsShared = 0;
    interrupts(); // we have local copies of the inputs, so now we can turn interrupts back on
    // as soon as interrupts are back on, we can no longer use the shared copies, the interrupt
    // service routines own these and could update them at any time. During the update, the
    // shared copies may contain junk. Luckily we have our local copies to work with :-)
  }
  
  ch1 = unChannel1In-1000; // Zero servo reading
  ch2 = unChannel2In-1500; // Zero servo reading
  ch3 = unChannel3In-1500; // Zero servo reading
  ch4 = unChannel4In-1500; // Zero servo reading

  if (ch1 < 0) {
    ch1 = 0;
  } 
  if (ch1 > 1000) {
    ch1 = 1000;
  }
  if (ch2 < -500) {
    ch2 = -500;
  } 
  if (ch2 > 500) {
    ch2 = 500;
  }
  if (ch3 < -500) {
    ch3 = -500;
  } 
  if (ch3 > 500) {
    ch3 = 500;
  }
  if (ch4 < -500) {
    ch4 = -500;
  } 
  if (ch4 > 500) {
    ch4 = 500;
  }

  // Print servo values to serial
  Serial.print(ch1);
  Serial.print(",");
  Serial.print(ch2);
  Serial.print(",");
  Serial.print(ch3);
  Serial.print(",");
  Serial.print(ch4);
  Serial.print(",");
}
      
void writeServos() {
  m1_cmd = min(m1_cmd,1000);
  m1_cmd = max(m1_cmd,0);
  m2_cmd = min(m2_cmd,1000);
  m2_cmd = max(m2_cmd,0);
  m3_cmd = min(m3_cmd,1000);
  m3_cmd = max(m3_cmd,0);
  m4_cmd = min(m4_cmd,1000);
  m4_cmd = max(m4_cmd,0);  
  servoChannel1.writeMicroseconds(1000 + m1_cmd);
  servoChannel2.writeMicroseconds(1000 + m2_cmd);
  servoChannel3.writeMicroseconds(1000 + m3_cmd);
  servoChannel4.writeMicroseconds(1000 + m4_cmd);
  

  Serial.print(m1_cmd);
  Serial.print(',');
  Serial.print(m2_cmd);
  Serial.print(',');
  Serial.print(m3_cmd);
  Serial.print(',');
  Serial.println(m4_cmd);
}
    
void setup() {
  Serial.begin(115200);

  // BMP085
  bmp.begin();
  
  // Turn on startup LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, true);
  
  // attach servo objects, these will generate the correct
  // pulses for driving Electronic speed controllers, servos or other devices
  // designed to interface directly with RC Receivers 
  servoChannel1.writeMicroseconds(1000);
  servoChannel2.writeMicroseconds(1000);
  servoChannel3.writeMicroseconds(1000);
  servoChannel4.writeMicroseconds(1000);
  servoChannel1.attach(CHANNEL1_OUT_PIN);
  servoChannel2.attach(CHANNEL2_OUT_PIN);
  servoChannel3.attach(CHANNEL3_OUT_PIN);
  servoChannel4.attach(CHANNEL4_OUT_PIN);

  
  // attach the interrupts used to read the channels
  attachInterrupt(CHANNEL1_IN_PIN, calcChannel1,CHANGE);
  attachInterrupt(CHANNEL2_IN_PIN, calcChannel2,CHANGE);
  attachInterrupt(CHANNEL3_IN_PIN, calcChannel3,CHANGE);
  attachInterrupt(CHANNEL4_IN_PIN, calcChannel4,CHANGE);
  
  // Power cycle the I2C devices after a reset.
  pinMode(I2C_PWR, OUTPUT);
  digitalWrite(I2C_PWR, false);
  delay(500);
  digitalWrite(I2C_PWR, true);
  
  // Initialize I2C
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif
  delay(1000);
  
  // Initialize Serial
  Serial.begin(115200);
  
  // Initialize MPU6050 accelgyro
  accelgyro.initialize();
      
  t_last = micros();
  
  digitalWrite(LED_PIN, false);
}

int countSplitCharacters(String text, char splitChar) {
 int returnValue = 0;
 int index = -1;
 
 while (index > -1) {
   index = text.indexOf(splitChar, index + 1);
 
   if(index > -1) returnValue+=1;
 }
}

void loop() {
  // Serial Read
  while (Serial.available()) {
    char c = Serial.read();
    if (c == 'a' && selectmotor == 0) { 
      selectmotor = 1;
    } else if (c == 'b' && selectmotor == 0) {
      selectmotor = 2;
    } else if (c == 'c' && selectmotor == 0) {
      selectmotor = 3;
    } else if (c == 'd' && selectmotor == 0) {
      selectmotor = 4;
    } else if (c == 'a' && selectmotor == 1) { 
      selectmotor = 0;
      m1_cmd = commandmotor.toInt();
      commandmotor = "";
    } else if (c == 'b' && selectmotor == 2) {
      selectmotor = 0;
      m2_cmd = commandmotor.toInt();
      commandmotor = "";
    } else if (c == 'c' && selectmotor == 3) {
      selectmotor = 0;
      m3_cmd = commandmotor.toInt();
      commandmotor = "";
    } else if (c == 'd' && selectmotor == 4) {
      selectmotor = 0;
      m4_cmd = commandmotor.toInt();
      commandmotor = "";
    } else if (commandmotor.length() > 3) {
      selectmotor = 0;
      commandmotor = "";
    } else if (selectmotor > 0 && c > 47 && c < 58) {
      commandmotor += c;
    } else if (c == 'x') {
      selectmotor = 0;
      commandmotor = "";
    }
  }
  
  if (abs(micros()-t_last) >= loop_period) {
    t_last = t_last + loop_period;
    
    // read angle (hundreths of a degree)
    int t_start = micros();
    readAngle();
    readAlt();
    readServos();
    writeServos();
    // Serial.println(micros()-t_start);
  }
}

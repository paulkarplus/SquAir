/* SquAir Quadcopter Arduino DUE Code
Copyright (c) 2014 Paul Karplus
This code is the controller for the SquAir quadcopter. 

*/

#include <Servo.h>
#include "math.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// Declare global variables
MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;
int16_t gx_zero = 0, gy_zero = 0, gz_zero = 0;
double a_roll_zero = 0, a_pitch_zero = 0; // thousandths of a degree
int t_last = 0;
double a_roll = 0, a_pitch = 0; // thousandths of a degree
double g_roll = 0, g_pitch = 0; // thousandths of a degree
double roll = 0, pitch = 0; // thousanths of a degree
double C = .05; // coefficient for accelerometer data in complimentary filter
int loop_period = 5000; // period of control loop in micros

// Define output servos
Servo servoChannel1;
Servo servoChannel2;
Servo servoChannel3;
Servo servoChannel4;

#define LED_PIN 13
#define I2C_PWR 32
// Assign your channel in pins
#define CHANNEL1_IN_PIN 2
#define CHANNEL2_IN_PIN 3
#define CHANNEL3_IN_PIN 4
#define CHANNEL4_IN_PIN 5

// Assign your channel out pins
#define CHANNEL1_OUT_PIN 10
#define CHANNEL2_OUT_PIN 11
#define CHANNEL3_OUT_PIN 12
#define CHANNEL4_OUT_PIN 13

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
static uint32_t unChannel1In;
static uint32_t unChannel2In;
static uint32_t unChannel3In;
static uint32_t unChannel4In;
// local copy of update flags
static uint32_t bUpdateFlags;

void calcChannel1()
{
  static uint32_t ulStart;
  if(digitalRead(CHANNEL1_IN_PIN)) {
    ulStart = micros();
  } else {
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
  } else
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

void zeroAccelGyro() {
  //int16_t ax_cal = 0, ay_cal = 0, az_cal = 0;
  
  int samples_to_ave = 100;
  for (int i = 0; i < samples_to_ave; i++) {
    // Accelerometer offsets
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    //ax_cal += ax; ay_cal += ay; az_cal += az;
    gx_zero += gx; gy_zero += gy; gz_zero += gz;
    // Gyro offsets
    double a_roll = atan2(ax,az)*57325; // thousandths of a degree
    double a_pitch = atan2(ay,az)*57325; // thousandths of a degree
    a_roll_zero += a_roll; a_pitch_zero += a_pitch;
    delay(5);
  }
  // Average Accelerometer offsets
  gx_zero = gx_zero/samples_to_ave;
  gy_zero = gy_zero/samples_to_ave;
  gz_zero = gz_zero/samples_to_ave;
  accelgyro.setXGyroOffset(0);
  accelgyro.setYGyroOffset(0);
  accelgyro.setZGyroOffset(0);
  
  // Average Gyro offsets
  a_roll_zero = a_roll_zero/samples_to_ave;
  a_pitch_zero = a_pitch_zero/samples_to_ave;
}

void readAngle() {
  // read raw accel/gyro measurements from device
  int t = millis();
  int t_micro = micros();
  
  // Read accelgyro data
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  
  // Calculate Accel Angle
  double a_roll = -1*(atan2(ax,az)*57325-a_roll_zero); // thousandths of a degree
  double a_pitch = (atan2(ay,az)*57325-a_pitch_zero); // thousandths of a degree
  
  // Calculate Gyro Angle
  double gx_cal = gx-gx_zero;
  double gy_cal = gy-gy_zero;
  double g_roll_delta = gy_cal*500*(double)loop_period/1000/65535; 
  double g_pitch_delta = gx_cal*500*(double)loop_period/1000/65535;
  g_roll += g_roll_delta; // thousandths of a degree
  g_pitch += g_pitch_delta; // thousandths of a degree
  
  // Calculate Complementary Filter Angle
  roll += C*(a_roll-roll) + (1-C)*g_roll_delta;
  pitch += C*(a_pitch-pitch) + (1-C)*g_pitch_delta;
  
  Serial.print(t);
  Serial.print(",");
  //Serial.print(micros()-t_micro);
  /*Serial.print(",");
  Serial.print((int)a_roll);
  Serial.print(",");
  Serial.print((int)g_roll);
  Serial.print(",");
  Serial.print((int)roll);
  Serial.print(",");
  Serial.print((int)a_pitch);
  Serial.print(",");
  Serial.print((int)g_pitch);
  Serial.print(",");
  Serial.println((int)pitch);
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

  Serial.print(unChannel1In);
  Serial.print(",");
  Serial.print(unChannel2In);
  Serial.print(",");
  Serial.print(unChannel2In);
  Serial.print(",");
  Serial.print(unChannel3In);
  Serial.print(",");
  Serial.print(unChannel4In);
  Serial.print(",");
  Serial.println(unChannel4In);
}
  
    
    
void setup() {
  Serial.begin(115200);

  // Turn on startup LED
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, true);
  
  // attach servo objects, these will generate the correct
  // pulses for driving Electronic speed controllers, servos or other devices
  // designed to interface directly with RC Receivers 
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
  //Serial.println("Initializing I2C devices...");
  accelgyro.initialize();
  // verify connection
  //Serial.println("Testing device connections...");
  //Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
  // zero accelgyro
  zeroAccelGyro();
  //Serial.print(a_roll_zero);
  //Serial.print(" , ");
  //Serial.println(a_pitch_zero);
      
  t_last = micros();
  digitalWrite(LED_PIN, false);
}

void loop() {
  
  if (abs(micros()-t_last) >= loop_period) {
    t_last = t_last + loop_period;
    // read angle (thousandths of a degree)
    readAngle();
    readServos();
  }
}

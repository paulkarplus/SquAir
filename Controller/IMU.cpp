// IMU object
// 2/26/2014 by Paul Karplus <pkarplus@gmail.com>

/* ============================================
Copyright (c) 2014 by Paul Karplus
*/

#include "IMU.h"

/** Default constructor, uses default I2C address.
 */
IMU::IMU() {
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
	// Initialize MPU6050 accelgyro
	Serial.println("Initializing I2C devices...");
	accelgyro.initialize();
	// verify connection
	Serial.println("Testing device connections...");
	Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");
	// zero accelgyro
	zeroAccelGyro();
	//Serial.print(a_roll_zero);
	//Serial.print(" , ");
	//Serial.println(a_pitch_zero);
}

/** Specific address constructor.
 */
void IMU::zeroAccelGyro() {
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

void IMU::getRollPitch(double* roll, double* pitch) {
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
  *roll += C*(a_roll-roll) + (1-C)*g_roll_delta;
  *pitch += C*(a_pitch-pitch) + (1-C)*g_pitch_delta;
  
  Serial.print(t);
  //Serial.print(",");
  //Serial.print(micros()-t_micro);
  Serial.print(",");
  Serial.print((int)a_roll);
  Serial.print(",");
  Serial.print((int)g_roll);
  Serial.print(",");
  Serial.print((int)*roll);
  Serial.print(",");
  Serial.print((int)a_pitch);
  Serial.print(",");
  Serial.print((int)g_pitch);
  Serial.print(",");
  Serial.println((int)*pitch);
}



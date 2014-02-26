// IMU object
// 2/26/2014 by Paul Karplus <pkarplus@gmail.com>

/* ============================================
Copyright (c) 2014 by Paul Karplus
*/

#ifndef _IMU_H_
#define _IMU_H_

#include "math.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

#define I2C_PWR_PIN 32

class IMU {
    public:
        IMU();
        
        // Get IMU Roll and Pitch Angles in thousandths of a degree using a complementary filter.
		void getRollPitch(double* roll, double* pitch);		

    private:
		void zeroAccelGyro();
        uint8_t devAddr;
        uint8_t buffer[14];
		int16_t ax, ay, az;
		int16_t gx, gy, gz;
		int16_t gx_zero = 0, gy_zero = 0, gz_zero = 0;
		double a_roll_zero = 0, a_pitch_zero = 0; // thousandths of a degree
		MPU6050 accelgyro;
		double a_roll = 0, a_pitch = 0; // thousandths of a degree
		double g_roll = 0, g_pitch = 0; // thousandths of a degree
		//double roll = 0, pitch = 0; // thousanths of a degree
		double C = .05; // coefficient for accelerometer data in complimentary filter

};

#endif /* _IMU_H_ */

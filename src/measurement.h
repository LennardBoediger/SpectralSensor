#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <sys/time.h> 
#include <math.h>
#include "../lib/wiringPi_AS726X_Libary/AS726X.h"
#include "../lib/influxDB_http_Libary/influxdb.h"

#define GAIN_POSSIBILITIES 3

// Write settings to device 
// Input taget i2c address - integrationValue and gain
void settings(int address, uint8_t integrationValue, uint8_t gain);


// Multiplys measurment values to match them to the maximum Gain(x64)
// Input value to match and gain used to measure this value
// Returns the matched value
uint32_t matchValueToMaxGain(int used_gain, uint32_t value);

// Save the biggest (gain dependet) AS7261 value that is not clipping
// Input AS7261 measurements in a AS7261_channel struct
// Returns the Gain of the value that was saved
AS7261_channel cleanAS7261Data(AS7261_channel *const AS7261_measurement);

// Save the biggest (gain dependet) AS7265X value that is not clipping
// Input AS7265X measurements in a AS7265X_channel struct
// Returns the Gain of the value that was saved
AS7265X_channel cleanAS7265XData(AS7265X_channel *const AS7265X_measurement);

// Read Values from AS7261 to AS7261_channel data structure
// Input: Sensor I2C Adress , Time the measurement was taken (epoch time in ms), gain of the measurement, 
//        instant_db_save = 1 -> save data instantly to the database 0 -> just return data
// Return filled AS7261_channel data structure
AS7261_channel getAS7261Measurement(int address, uint64_t measurement_time, uint8_t gain, uint8_t instant_db_save);

// Read Values from AS7265X to AS7265X_channel data structure
// Input: Sensor I2C Adress , Time the measurement was taken (epoch time in ms), gain of the measurement, 
//        instant_db_save = 1 -> save data instantly to the database 0 -> just return data
// Return filled AS7265X_channel data structure
AS7265X_channel getAS7265XMeasurement(int address, uint64_t measurement_time, uint8_t gain, uint8_t instant_db_save);


// write AS7261 Measurement Values to database (InfluxDB)
// Input: Sensor I2C Adress, Measurement Values in AS7261_channel data structure, 
//        Time the measurement was taken (epoch time in ms)
// no return value code will crash with Error: 500 Write to Database Failed
// no problem during testing, but should be improved!
void saveAS7261Measurement(int address,AS7261_channel values, uint64_t measurement_time);

// write AS7265X Measurement Values to database (InfluxDB)
// Input: Sensor I2C Adress, Measurement Values in AS7265X_channel data structure, 
//        Time the measurement was taken (epoch time in ms)
// no return value code will crash with Error: 500 Write to Database Failed
// no problem during testing, but should be improved!
void saveAS7265XMeasurement(int address, AS7265X_channel values, uint64_t measurement_time);

// write AS7261 Used Gain Values to database (InfluxDB)
// Input: Sensor I2C Adress, Used Gain Values in AS7261_channel data structure, 
//        Time the measurement was taken (epoch time in ms)
void saveAS7261Gain(int address,AS7261_channel gain, uint64_t measurement_time);

// Takes Measurement of selcted AS7261 Devices in Auto Gain Mode and saves them to the database
// Input: Current epoch time in ms, pointer to sensor_list struct of selcted AS7261 Devices, 
//        integrationValue [0:255]
void autoGainMeasurementAS7261(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue);

// write AS7261 Used Gain Values to database (InfluxDB)
// Input: Sensor I2C Adress, Used Gain Values in AS7261_channel data structure, 
//        Time the measurement was taken (epoch time in ms)
void saveAS7265XGain(int address,AS7265X_channel gain, uint64_t measurement_time);

// Takes Measurement of selcted AS7265X Devices in Auto Gain Mode and saves them to the database
// Input: Current epoch time in ms, pointer to sensor_list struct of selcted AS7265X Devices, 
//        integrationValue [0:255]
void autoGainMeasurementAS7265X(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue);

// Takes Measurement of selcted AS7261 Devices in Fixed Gain Mode and saves them to the database
// Input: Current epoch time in ms, pointer to sensor_list struct of selcted AS7261 Devices, 
//        integrationValue [0:255], Gain [0:3]
void fixedGainMeasurementAS7261(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue, uint8_t gain);

// Takes Measurement of selcted AS7265X Devices in Fixed Gain Mode and saves them to the database
// Input: Current epoch time in ms, pointer to sensor_list struct of selcted AS7265X Devices, 
//        integrationValue [0:255], Gain [0:3]
void fixedGainMeasurementAS7265X(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue, uint8_t gain);
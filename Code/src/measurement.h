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

void settings(int address, uint8_t integrationValue, uint8_t gain);

uint32_t matchValueToMaxGain(int used_gain, uint32_t value);
AS7261_channel cleanAS7261Data(AS7261_channel *const AS7261_measurement);
AS7265X_channel cleanAS7265XData(AS7265X_channel *const AS7265X_measurement);
AS7261_channel getAS7261Measurement(int address, uint64_t measurement_time, uint8_t gain, uint8_t instant_db_save);
AS7265X_channel getAS7265XMeasurement(int address, uint64_t measurement_time, uint8_t gain, uint8_t instant_db_save);
void saveAS7261Measurement(int address,AS7261_channel values, uint64_t measurement_time);
void saveAS7265XMeasurement(int address, AS7265X_channel values, uint64_t measurement_time);
void saveAS7261Gain(int address,AS7261_channel gain, uint64_t measurement_time);
void autoGainMeasurementAS7261(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue);
void saveAS7265XGain(int address,AS7265X_channel gain, uint64_t measurement_time);
void autoGainMeasurementAS7265X(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue);
void fixedGainMeasurementAS7261(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue, uint8_t gain);
void fixedGainMeasurementAS7265X(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue, uint8_t gain);
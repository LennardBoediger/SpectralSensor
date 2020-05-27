#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

float getCalibratedValue(uint8_t calAddress, int fd);
float convertBytesToFloat(uint32_t myLong);
uint8_t dataAvailable(int fd);
void clearDataAvailable(int fd);
void enableIndicator(int fd);
void disableIndicator(int fd);
void setIndicatorCurrent(uint8_t current, int fd);
void enableBulb(int fd);
void disableBulb (int fd);
void setBulbCurrent(uint8_t current , int fd);
uint8_t getTemperature(int fd);
float getTemperatureF(int fd);
void softReset(int fd);
uint8_t virtualReadRegister(uint8_t virtualAddr, int fd);
void virtualWriteRegister(uint8_t virtualAddr, uint8_t dataToWrite, int fd);










#define AS726X_ADDR 0x49 //7-bit unshifted default I2C Address
#define SENSORTYPE_AS7262 0x3E
#define SENSORTYPE_AS7263 0x3F

//Register addresses
#define AS726x_DEVICE_TYPE 0x00
#define AS726x_HW_VERSION 0x01
#define AS726x_CONTROL_SETUP 0x04
#define AS726x_INT_T 0x05
#define AS726x_DEVICE_TEMP 0x06
#define AS726x_LED_CONTROL 0x07

#define AS72XX_SLAVE_STATUS_REG 0x00
#define AS72XX_SLAVE_WRITE_REG 0x01
#define AS72XX_SLAVE_READ_REG 0x02

//The same register locations are shared between the AS7262 and AS7263, they're just called something different
//AS7262 Registers
#define AS7262_V 0x08
#define AS7262_B 0x0A
#define AS7262_G 0x0C
#define AS7262_Y 0x0E
#define AS7262_O 0x10
#define AS7262_R 0x12
#define AS7262_V_CAL 0x14
#define AS7262_B_CAL 0x18
#define AS7262_G_CAL 0x1C
#define AS7262_Y_CAL 0x20
#define AS7262_O_CAL 0x24
#define AS7262_R_CAL 0x28

//AS7263 Registers
#define AS7263_R 0x08
#define AS7263_S 0x0A
#define AS7263_T 0x0C
#define AS7263_U 0x0E
#define AS7263_V 0x10
#define AS7263_W 0x12
#define AS7263_R_CAL 0x14
#define AS7263_S_CAL 0x18
#define AS7263_T_CAL 0x1C
#define AS7263_U_CAL 0x20
#define AS7263_V_CAL 0x24
#define AS7263_W_CAL 0x28

#define AS72XX_SLAVE_TX_VALID 0x02
#define AS72XX_SLAVE_RX_VALID 0x01

#define SENSORTYPE_AS7262 0x3E
#define SENSORTYPE_AS7263 0x3F

#define POLLING_DELAY 5 //Amount of ms to wait between checking for virtual register changes
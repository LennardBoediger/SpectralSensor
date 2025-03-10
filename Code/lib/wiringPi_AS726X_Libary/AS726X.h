#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <unistd.h> // for close

//Sensor Type Indentifiers
#define SENSORTYPE_AS7261 61
#define SENSORTYPE_AS72651 65

#define AS72XX_SLAVE_TX_VALID 0x02
#define AS72XX_SLAVE_RX_VALID 0x01

//Physical Register addresses
//TODO refcractor name: remove slave
#define AS72XX_SLAVE_STATUS_REG 0x00
#define AS72XX_SLAVE_WRITE_REG 0x01
#define AS72XX_SLAVE_READ_REG 0x02

//Virtual Register addresses
#define AS726x_DEVICE_TYPE 0x00
#define AS726x_HW_VERSION 0x01
#define AS726x_CONTROL_SETUP 0x04
#define AS726x_INT_T 0x05
#define AS726x_DEVICE_TEMP 0x06
#define AS726x_LED_CONTROL 0x07

//AS7261 Registers
//Raw channel registers
#define AS7261_X 0x08
#define AS7261_Y 0x0A
#define AS7261_Z 0x0C
#define AS7261_NIR 0x0E
#define AS7261_DARK 0x10
#define AS7261_CLEAR 0x12

//AS7265X
//Raw channel registers
#define AS7265X_R_G_A     0x08
#define AS7265X_S_H_B     0x0A
#define AS7265X_T_I_C     0x0C
#define AS7265X_U_J_D     0x0E
#define AS7265X_V_K_E     0x10
#define AS7265X_W_L_F     0x12


//AS7265X Device Selection
#define AS7265X_DEV_SELECT_CONTROL  0x4F
#define AS72652_ACTIVE  4 // Bit 4 of AS7265X_DEV_SELECT_CONTROL is set if AS72652 is detected
#define AS72653_ACTIVE  5 // Bit 5 of AS7265X_DEV_SELECT_CONTROL is set if AS72653 is detected
//AS7265X Device Selcetors
#define AS72651_id    	0x00
#define AS72652_id      0x01
#define AS72653_id      0x02

#define POLLING_DELAY 5 //Amount of ms to wait between checking for virtual register changes

#ifndef GRANDPARENT_H
#define GRANDPARENT_H

struct AS7261_channel{
	uint32_t X;
	uint32_t Y;
	uint32_t Z;
	uint32_t Clear;
	uint32_t Dark;
	uint32_t NIR;
};
typedef struct AS7261_channel AS7261_channel;

struct AS7265X_channel{
	uint32_t R;
	uint32_t S;
	uint32_t T;
	uint32_t U;
	uint32_t V;
	uint32_t W;
	uint32_t G;
	uint32_t H;
	uint32_t I;
	uint32_t J;
	uint32_t K;
	uint32_t L;
	uint32_t A;
	uint32_t B;
	uint32_t C;
	uint32_t D;
	uint32_t E;
	uint32_t F;
};
typedef struct AS7265X_channel AS7265X_channel;

struct sensor {
	int8_t mux_channel;
	int8_t address;
  	int8_t type;
  	int8_t num_device_addr; //number of devices under this address
};
typedef struct sensor sensor_list;


#endif /* GRANDPARENT_H */

// Unused Test Funktion 
uint8_t begin(uint8_t gain, uint8_t measurementMode, int fd);


void change_mux_channel(int8_t mux_channel);

// returns the Sensor Version
// AS7261 or AS7265X
uint8_t getVersion(int fd); //61 oder 65

void I2C_Mux_Scan(sensor_list *const s);

// Scanns for sensors on all 128 possible addresses
// input pointer to to array of sensor_list struct size hast to be 128
// wirtes sensor address and type to array of sensor_list struct
void I2C_Scan(sensor_list *const s, int8_t mux_channel);

//Test if Slave1 and 2 are detected.  Datasheet is wrong! AS72652 -> Bit 4, AS72653 -> Bit 5
uint8_t scan_AS7262(int fd);
uint8_t scan_AS7263(int fd);

//Select witch AS7265X Device is used
//AS72651_id or AS72652_id or AS72653_id 
void selectDevice(uint8_t device, int fd);

//Sets the measurement mode
//Mode 0: Continuous reading of VBGY (7262) / STUV (7263)
//Mode 1: Continuous reading of GYOR (7262) / RTUX (7263)
//Mode 2: Continuous reading of all channels (power-on default)
//Mode 3: One-shot reading of all channels
void setMeasurementMode(uint8_t mode, int fd);

//Sets the gain value
//Gain 0: 1x (power-on default)
//Gain 1: 3.7x
//Gain 2: 16x
//Gain 3: 64x
void setGain(uint8_t gain, int fd);

//Sets the integration value
//Give this function a uint8_t from 0 to 255.
//Time will be 2.8ms * [integration value]
void setIntegrationTime(uint8_t integrationValue, int fd);

//Disables the interrupt pin witch is not connected so disable it!
void disableInterrupt(int fd);

//Tells IC to take measurements and polls for data ready flag
void takeMeasurements(int fd);

//Calls takeMeasurements on gives I2C Adrress
void MeasurementFromAdress(int address);


//Get RAW AS7261 readings
int getX(int fd);
int getY(int fd);
int getZ(int fd);
int getNIR(int fd);
int getDark(int fd);
int getClear(int fd);

//Get RAW AS72651(NIR) readings
int getR(int fd);
int getS(int fd);
int getT(int fd);
int getU(int fd);
int getV(int fd);
int getW(int fd);

//Get RAW AS72652(color) readings
int getG(int fd);
int getH(int fd);
int getI(int fd);
int getJ(int fd);
int getK(int fd);
int getL(int fd);

//Get RAW AS72653(UV) readings
int getA(int fd);
int getB(int fd);
int getC(int fd);
int getD(int fd);
int getE(int fd);
int getF(int fd);


// returns Color channel of AS7265X
// returns -1 if slave AS72651 or AS72652 is not available
int getChannel_AS7265X(int device, uint8_t channelRegister, int fd);

//Returns a 16-bit value stored in two consecutive 8-Bit registers 
int getChannel(uint8_t channelRegister, int fd);

//Checks to see if DRDY flag is set in the control setup register
uint8_t dataAvailable(int fd);

//Clears the DRDY flag
//Normally this should clear when data registers are read
void clearDataAvailable(int fd);

//Enable the onboard indicator LED
void enableIndicator(int fd);

//Disable the onboard indicator LED
void disableIndicator(int fd);

//Returns the temperature in C 
uint8_t getTemperature(int fd); // very inaccurate readings!

//Does a soft reset
//Give sensor at least 1000ms to reset
void softReset(int fd);

//Read a virtual register from the AS726x
uint8_t virtualReadRegister(uint8_t virtualAddr, int fd);

//Write to a virtual register in the AS726x
void virtualWriteRegister(uint8_t virtualAddr, uint8_t dataToWrite, int fd);








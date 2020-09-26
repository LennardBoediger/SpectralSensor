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
//TODO remove slave
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

struct AS7261_channel{
	uint32_t X;
	uint32_t Y;
	uint32_t Z;
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
	int8_t address;
  	int8_t type;
  	int8_t num_device_addr; //number of devices under this address
  	AS7261_channel calibration_factor_AS7261[4];
  	AS7265X_channel calibration_factor_AS7265X[4];
};
typedef struct sensor sensor_list;


uint8_t begin(uint8_t gain, uint8_t measurementMode, int fd);//TODO remove unused function
uint8_t getVersion(int fd); //61 oder 65
void I2C_Scan(sensor_list *const s); //Remove _
uint8_t scan_AS7262(int fd);//Remove _
uint8_t scan_AS7263(int fd);//Remove _
void selectDevice(uint8_t device, int fd);
void setMeasurementMode(uint8_t mode, int fd);
void setGain(uint8_t gain, int fd);
void setIntegrationTime(uint8_t integrationValue, int fd);
void disableInterrupt(int fd);
void takeMeasurements(int fd);
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

//Get the various UV readings
//Get RAW AS72653(UV) readings
int getA(int fd);
int getB(int fd);
int getC(int fd);
int getD(int fd);
int getE(int fd);
int getF(int fd);

int getChannel_AS7265X(int device, uint8_t channelRegister, int fd);
int getChannel(uint8_t channelRegister, int fd);

uint8_t dataAvailable(int fd);
void clearDataAvailable(int fd);
void enableIndicator(int fd);
void disableIndicator(int fd);
uint8_t getTemperature(int fd);//latex
void softReset(int fd);
uint8_t virtualReadRegister(uint8_t virtualAddr, int fd);
void virtualWriteRegister(uint8_t virtualAddr, uint8_t dataToWrite, int fd);








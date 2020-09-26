#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <string.h>
#include <unistd.h> // for close
#include "AS726X.h"


uint8_t begin(uint8_t gain, uint8_t measurementMode, int fd){
    disableIndicator(fd);
    setIntegrationTime(50, fd); //50 * 2.8ms = 140ms. 0 to 255 is valid.
    //If you use Mode 2 or 3 (all the colors) then integration time is double. 140*2 = 280ms between readings.

    printf("Sensor Version: %d\n",getVersion(fd));
    setGain(gain, fd); //Set gain to 64x
    setMeasurementMode(measurementMode, fd); //One-shot reading of VBGYOR

    return 1;
}


// returns the Sensor Version
// AS7261 or AS7265X
uint8_t getVersion(int fd) {
    return virtualReadRegister(AS726x_HW_VERSION, fd);
}
 
// Scanns for sensors on all 128 possible addresses
// input pointer to to array of sensor_list struct size hast to be 128
// wirtes sensor address and type to array of sensor_list struct
void I2C_Scan(sensor_list *const s){
    //printf("test struct address in fuction%i is value %i\n",s[0].address, s[0].type );
    printf("----------I2C Scan ----------\n");
    uint8_t sensor_count = 0;
    for (int address = 0; address < 128; ++address){
        int fd =  wiringPiI2CSetup(address);
        if (fd != -1){
            //try to write to some hopefully unused register(5)-> return value: 0 indicates that someone was listening 
            //this possibly writes to the Integration Value Register so make shure to properly set is before starting a measurement
            if (wiringPiI2CWriteReg8 (fd, 5, 1) == 0){  // 0 indicates sucsessful writing oparation
                int version = getVersion(fd);   
                if (version == SENSORTYPE_AS7261){
                    printf("Device at: 0x%X is AS7261\n",address);
                    s[sensor_count].address = address;
                    s[sensor_count].type = SENSORTYPE_AS7261;
                    s[sensor_count].num_device_addr = 1;
                    sensor_count++;
                }
                else if(version == SENSORTYPE_AS72651){
                    printf("Device at: 0x%X is AS72651",address);
                    s[sensor_count].address = address;
                    s[sensor_count].type = SENSORTYPE_AS72651;
                    s[sensor_count].num_device_addr = 1;
                    if (scan_AS7262(fd)){
                        s[sensor_count].num_device_addr += 1;
                        printf(", AS72652");
                    }
                    if (scan_AS7263(fd)){
                        s[sensor_count].num_device_addr += 1;
                        printf(", AS72653");
                    }
                    sensor_count++;
                    printf("\n");
                }
            }
        }
        close(fd);
    }
    printf("-----------------------------\n");
}


//Test if Slave1 and 2 are detected.  Datasheet is wrong! AS72652 -> Bit 4, AS72653 -> Bit 5
uint8_t scan_AS7262(int fd) {
    uint8_t value = virtualReadRegister(AS7265X_DEV_SELECT_CONTROL, fd);
    if ((value & 0b00010000) == 1 << AS72652_ACTIVE){
        return 1;
    }
    else{
        return 0;
    }
}

uint8_t scan_AS7263(int fd) {
    uint8_t value = virtualReadRegister(AS7265X_DEV_SELECT_CONTROL, fd);
    if ((value & 0b00100000) == 1 << AS72653_ACTIVE){
        return 1;
    }
    else{
        return 0;
    }
}

//Select witch AS7265X Device is used
//AS72651_id or AS72652_id or AS72653_id  
void selectDevice(uint8_t device, int fd) {
  //Set the bits 0:1. Just overwrite whatever is there because masking in the correct value doesn't work.
  virtualWriteRegister(AS7265X_DEV_SELECT_CONTROL, device, fd);
}

//Sets the measurement mode
//Mode 0: Continuous reading of VBGY (7262) / STUV (7263)
//Mode 1: Continuous reading of GYOR (7262) / RTUX (7263)
//Mode 2: Continuous reading of all channels (power-on default)
//Mode 3: One-shot reading of all channels
void setMeasurementMode(uint8_t mode, int fd) {
    if (mode > 0b11) mode = 0b11; //wrong value will be set to Mode 3

    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_CONTROL_SETUP, fd); //Read
    value &= 0b11110011; //Clear BANK bits
    value |= (mode << 2); //Set BANK bits with user's choice
    virtualWriteRegister(AS726x_CONTROL_SETUP, value, fd); //Write
}

//Sets the gain value
//Gain 0: 1x (power-on default)
//Gain 1: 3.7x
//Gain 2: 16x
//Gain 3: 64x
void setGain(uint8_t gain, int fd) {
    if (gain > 0b11) gain = 0b11;

    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_CONTROL_SETUP, fd); //Read
    value &= 0b11001111; //Clear GAIN bits
    value |= (gain << 4); //Set GAIN bits with user's choice
    virtualWriteRegister(AS726x_CONTROL_SETUP, value, fd); //Write
}

//Sets the integration value
//Give this function a uint8_t from 0 to 255.
//Time will be 2.8ms * [integration value]
void setIntegrationTime(uint8_t integrationValue, int fd) {
    virtualWriteRegister(AS726x_INT_T, integrationValue, fd); //Write
}

//Disables the interrupt pin witch is not connected so disable it!
void disableInterrupt(int fd) {
    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_CONTROL_SETUP, fd); //Read
    value &= 0b10111111; //Clear INT bit
    virtualWriteRegister(AS726x_CONTROL_SETUP, value, fd); //Write
}

//Tells IC to take measurements and polls for data ready flag
void takeMeasurements(int fd) {
    clearDataAvailable(fd); //Clear DATA_RDY flag when using Mode 3

    //Goto mode 3 for one shot measurement of all channels
    setMeasurementMode(3, fd);

    //Wait for data to be ready
    while (dataAvailable(fd) == 0) delay(POLLING_DELAY); //Potential TODO: avoid this to get faster nearly parralel mesurments

    //Readings can now be accessed via getViolet(), getBlue(), etc
}

//Calls takeMeasurements on gives I2C Adrress
void MeasurementFromAdress(int address){
    int fd =  wiringPiI2CSetup(address);
    if (fd == -1) {
        printf("i2c failed");
    }
    takeMeasurements(fd);  // takesMeasurmant Readings can now be accessed via getX(), getY(), etc
    close(fd);
}

//Get RAW AS7261 readings
int getX(int fd) { return(getChannel(AS7261_X, fd));}
int getY(int fd) { return(getChannel(AS7261_Y, fd));}
int getZ(int fd) { return(getChannel(AS7261_Z, fd));}
int getNIR(int fd) { return(getChannel(AS7261_NIR, fd));}
int getDark(int fd) { return(getChannel(AS7261_DARK, fd));}
int getClear(int fd) { return(getChannel(AS7261_CLEAR, fd));}

//Get RAW AS72651(NIR) readings
int getR(int fd) { return(getChannel_AS7265X(AS72651_id, AS7265X_R_G_A, fd));}
int getS(int fd) { return(getChannel_AS7265X(AS72651_id, AS7265X_S_H_B, fd));}
int getT(int fd) { return(getChannel_AS7265X(AS72651_id, AS7265X_T_I_C, fd));}
int getU(int fd) { return(getChannel_AS7265X(AS72651_id, AS7265X_U_J_D, fd));}
int getV(int fd) { return(getChannel_AS7265X(AS72651_id, AS7265X_V_K_E, fd));}
int getW(int fd) { return(getChannel_AS7265X(AS72651_id, AS7265X_W_L_F, fd));}

//Get RAW AS72652(color) readings
int getG(int fd) { return(getChannel_AS7265X(AS72652_id, AS7265X_R_G_A, fd));}
int getH(int fd) { return(getChannel_AS7265X(AS72652_id, AS7265X_S_H_B, fd));}
int getI(int fd) { return(getChannel_AS7265X(AS72652_id, AS7265X_T_I_C, fd));}
int getJ(int fd) { return(getChannel_AS7265X(AS72652_id, AS7265X_U_J_D, fd));}
int getK(int fd) { return(getChannel_AS7265X(AS72652_id, AS7265X_V_K_E, fd));}
int getL(int fd) { return(getChannel_AS7265X(AS72652_id, AS7265X_W_L_F, fd));}

//Get the various UV readings
//Get RAW AS72653(UV) readings
int getA(int fd) { return(getChannel_AS7265X(AS72653_id, AS7265X_R_G_A, fd));}
int getB(int fd) { return(getChannel_AS7265X(AS72653_id, AS7265X_S_H_B, fd));}
int getC(int fd) { return(getChannel_AS7265X(AS72653_id, AS7265X_T_I_C, fd));}
int getD(int fd) { return(getChannel_AS7265X(AS72653_id, AS7265X_U_J_D, fd));}
int getE(int fd) { return(getChannel_AS7265X(AS72653_id, AS7265X_V_K_E, fd));}
int getF(int fd) { return(getChannel_AS7265X(AS72653_id, AS7265X_W_L_F, fd));}

// returns Color channel of AS7265X
// returns -1 if slave AS72651 or AS72652 is not available
int getChannel_AS7265X(int device, uint8_t channelRegister, int fd){
    selectDevice(AS72651_id, fd);   //select AS72651 to verify presence of slave sensors
    if (device == AS72651_id){
        return getChannel(channelRegister, fd);
    }
    else if(device == AS72652_id && scan_AS7262(fd)){
        selectDevice(device, fd);
        return getChannel(channelRegister, fd);
    }
    else if (device == AS72653_id && scan_AS7263(fd)){
        selectDevice(device, fd);
        return getChannel(channelRegister, fd);
    }
    return -1;
}

//Returns a 16-bit value stored in two consecutive 8-Bit registers 
int getChannel(uint8_t channelRegister, int fd){
    int colorData = virtualReadRegister(channelRegister , fd) << 8; //High uint8_t
    colorData |= virtualReadRegister(channelRegister + 1, fd); //Low uint8_t
    return(colorData);
}

//Checks to see if DRDY flag is set in the control setup register
uint8_t dataAvailable(int fd) {
    uint8_t value = virtualReadRegister(AS726x_CONTROL_SETUP, fd);
    return (value & (1 << 1)); //Bit 1 is DATA_RDY
}

//Clears the DRDY flag
//Normally this should clear when data registers are read
void clearDataAvailable(int fd) {
    uint8_t value = virtualReadRegister(AS726x_CONTROL_SETUP, fd);
    value &= ~(1 << 1); //Set the DATA_RDY bit
    virtualWriteRegister(AS726x_CONTROL_SETUP, value, fd);
}

//Enable the onboard indicator LED
void enableIndicator(int fd) {
    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_LED_CONTROL, fd);
    value |= (1 << 0); //Set the bit
    virtualWriteRegister(AS726x_LED_CONTROL, value ,fd);
}

//Disable the onboard indicator LED
void disableIndicator(int fd) {
    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_LED_CONTROL , fd);
    value &= ~(1 << 0); //Clear the bit
    virtualWriteRegister(AS726x_LED_CONTROL, value, fd);
}

//Returns the temperature in C
uint8_t getTemperature(int fd){
    return (virtualReadRegister(AS726x_DEVICE_TEMP, fd));
}

//Does a soft reset
//Give sensor at least 1000ms to reset
void softReset(int fd){
    uint8_t value = virtualReadRegister(AS726x_CONTROL_SETUP,fd); //Read
    value |= (1 << 7); //Set RST bit
    virtualWriteRegister(AS726x_CONTROL_SETUP, value, fd); //Write
}

//Read a virtual register from the AS726x
uint8_t virtualReadRegister(uint8_t virtualAddr, int fd){
    uint8_t  status;
    //Do a prelim check of the read register
    status =  wiringPiI2CReadReg8 (fd, AS72XX_SLAVE_STATUS_REG);
    if ((status & AS72XX_SLAVE_RX_VALID) != 0){ //There is data to be read
        /*uint8_t incoming = */wiringPiI2CReadReg8(fd, AS72XX_SLAVE_READ_REG); //Read the uint8_t but do nothing with it
    }
    //Wait for WRITE flag to clear
    while (1) {
        status = wiringPiI2CReadReg8(fd, AS72XX_SLAVE_STATUS_REG);
        if ((status & AS72XX_SLAVE_TX_VALID) == 0){
            break; // If TX bit is clear, it is ok to write
        }
        delay(POLLING_DELAY);
    }
    // Send the virtual register address (bit 7 should be 0 to indicate we are reading a register).
    wiringPiI2CWriteReg8 (fd, AS72XX_SLAVE_WRITE_REG, virtualAddr);

    //Wait for READ flag to be set
    while (1)
    {
        status = wiringPiI2CReadReg8(fd, AS72XX_SLAVE_STATUS_REG); 
        if ((status & AS72XX_SLAVE_RX_VALID) != 0) break; // Read data is ready.
        delay(POLLING_DELAY);
    }

    uint8_t incoming = wiringPiI2CReadReg8(fd, AS72XX_SLAVE_READ_REG);
    return (incoming);
}

//Write to a virtual register in the AS726x
void virtualWriteRegister(uint8_t virtualAddr, uint8_t dataToWrite, int fd){
    uint8_t status;
    //Wait for WRITE register to be empty
    while (1){
        status = wiringPiI2CReadReg8(fd,AS72XX_SLAVE_STATUS_REG);
        if((status & AS72XX_SLAVE_TX_VALID) == 0) {
            break; // No inbound TX pending at slave. Okay to write now.
        }
        delay(POLLING_DELAY);
    }
    // Send the virtual register address (setting bit 7 to indicate a  write a register).
    wiringPiI2CWriteReg8(fd, AS72XX_SLAVE_WRITE_REG, (virtualAddr | 0x80));

    //Wait for WRITE register to be empty
    while (1)
    {
        status = wiringPiI2CReadReg8(fd, AS72XX_SLAVE_STATUS_REG);
        if ((status & AS72XX_SLAVE_TX_VALID) == 0){
            break; // No inbound TX pending at slave. Okay to write now.
        }
        delay(POLLING_DELAY);
    }
    // Send the data to complete the operation.
    wiringPiI2CWriteReg8(fd, AS72XX_SLAVE_WRITE_REG, dataToWrite);
}

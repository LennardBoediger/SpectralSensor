#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <string.h>
#include "AS726X.h"

float AS726X::getCalibratedT() { return(getCalibratedValue(AS7263_T_CAL)); }
float AS726X::getCalibratedS() { return(getCalibratedValue(AS7263_S_CAL)); }
float AS726X::getCalibratedR() { return(getCalibratedValue(AS7263_R_CAL)); }
float AS726X::getCalibratedU() { return(getCalibratedValue(AS7263_U_CAL)); }
float AS726X::getCalibratedV() { return(getCalibratedValue(AS7263_V_CAL)); }
float getCalibratedW(int fd) { return(getCalibratedValue(AS7263_W_CAL, fd)); }

//Given an address, read four uint8_ts and return the floating point calibrated value
float getCalibratedValue(uint8_t calAddress, int fd)
{
    uint8_t b0, b1, b2, b3;
    b0 = virtualReadRegister(calAddress + 0, fd);
    b1 = virtualReadRegister(calAddress + 1, fd);
    b2 = virtualReadRegister(calAddress + 2, fd);
    b3 = virtualReadRegister(calAddress + 3, fd);

    //Channel calibrated values are stored big-endian
    uint32_t calBytes = 0;
    calBytes |= ((uint32_t)b0 << (8 * 3));
    calBytes |= ((uint32_t)b1 << (8 * 2));
    calBytes |= ((uint32_t)b2 << (8 * 1));
    calBytes |= ((uint32_t)b3 << (8 * 0));

    return (convertBytesToFloat(calBytes));
}

//Given 4 uint8_ts returns the floating point value
float convertBytesToFloat(uint32_t myLong)
{
    float myFloat;
    memcpy(&myFloat, &myLong, 4); //Copy uint8_ts into a float
    return (myFloat);
}

//Checks to see if DRDY flag is set in the control setup register
//TODO: was bool test retuned 2
uint8_t dataAvailable(int fd)
{
    uint8_t value = virtualReadRegister(AS726x_CONTROL_SETUP, fd);
    return (value & (1 << 1)); //Bit 1 is DATA_RDY
}

//Clears the DRDY flag
//Normally this should clear when data registers are read
void clearDataAvailable(int fd)
{
    uint8_t value = virtualReadRegister(AS726x_CONTROL_SETUP, fd);
    value &= ~(1 << 1); //Set the DATA_RDY bit
    virtualWriteRegister(AS726x_CONTROL_SETUP, value, fd);
}

//Enable the onboard indicator LED
void enableIndicator(int fd)
{
    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_LED_CONTROL, fd);
    value |= (1 << 0); //Set the bit
    virtualWriteRegister(AS726x_LED_CONTROL, value ,fd);
}

//Disable the onboard indicator LED
void disableIndicator(int fd)
{
    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_LED_CONTROL , fd);
    value &= ~(1 << 0); //Clear the bit
    virtualWriteRegister(AS726x_LED_CONTROL, value, fd);
}

//Set the current limit of onboard LED. Default is max 8mA = 0b11.
void setIndicatorCurrent(uint8_t current,int fd){
    if (current > 0b11) current = 0b11;
    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_LED_CONTROL, fd); //Read
    value &= 0b11111001; //Clear ICL_IND bits
    value |= (current << 1); //Set ICL_IND bits with user's choice
    virtualWriteRegister(AS726x_LED_CONTROL, value, fd); //Write
}

//Enable the onboard 5700k or external incandescent bulb
void enableBulb(int fd){
    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_LED_CONTROL,fd);
    value |= (1 << 3); //Set the bit
    virtualWriteRegister(AS726x_LED_CONTROL, value ,fd);
}

//Disable the onboard 5700k or external incandescent bulb
void disableBulb (int fd){
    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_LED_CONTROL, fd);
    value &= ~(1 << 3); //Clear the bit
    virtualWriteRegister(AS726x_LED_CONTROL, value, fd);
}


//Set the current limit of bulb/LED.
//Current 0: 12.5mA
//Current 1: 25mA
//Current 2: 50mA
//Current 3: 100mA
void setBulbCurrent(uint8_t current , int fd){
    if (current > 0b11) current = 0b11; //Limit to two bits

    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_LED_CONTROL, fd); //Read
    value &= 0b11001111; //Clear ICL_DRV bits
    value |= (current << 4); //Set ICL_DRV bits with user's choice
    virtualWriteRegister(AS726x_LED_CONTROL, value, fd); //Write
}

//Returns the temperature in C
//Pretty inaccurate: +/-8.5C //TODO: mabe include external Termometer to improve Readings
uint8_t getTemperature(int fd){
    return (virtualReadRegister(AS726x_DEVICE_TEMP, fd));
}

//Convert to F if needed
float getTemperatureF(int fd){
    float temperatureF = getTemperature(fd);
    temperatureF = temperatureF * 1.8 + 32.0;
    return (temperatureF);
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
        uint8_t incoming = wiringPiI2CReadReg8(fd, AS72XX_SLAVE_READ_REG); //Read the uint8_t but do nothing with it
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
        status = wiringPiI2CReadReg8(fd, AS72XX_SLAVE_STATUS_REG); //TODO: wirklich 8 reg?
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
    // Send the virtual register address (setting bit 7 to indicate we are writing to a register).
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

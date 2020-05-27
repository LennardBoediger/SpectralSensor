#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "AS726X.h"

void disableBulb (int fd){
    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_LED_CONTROL, fd);
    value &= ~(1 << 3); //Clear the bit
    virtualWriteRegister(AS726x_LED_CONTROL, value, fd);
}
void enableBulb(int fd){
    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_LED_CONTROL,fd);
    value |= (1 << 3); //Set the bit
    virtualWriteRegister(AS726x_LED_CONTROL, value ,fd);
}

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

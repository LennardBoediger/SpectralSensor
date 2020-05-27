#include <stdio.h>
#include <stdint.h>
#include "wiringPi.h"
#include <wiringPiI2C.h>

#define DEVICE_ID 0x49
#define AS726x_DEVICE_TEMP 0x06

#define AS72XX_SLAVE_STATUS_REG 0x00
#define AS72XX_SLAVE_WRITE_REG 0x01
#define AS72XX_SLAVE_READ_REG 0x02

#define AS72XX_SLAVE_TX_VALID 0x02
#define AS72XX_SLAVE_RX_VALID 0x01

#define POLLING_DELAY 5 //Amount of ms to wait between checking for virtual register changes


int TestDevice(int id){
    int fd = wiringPiI2CSetup(DEVICE_ID);
    //int fd = wiringPiI2CSetupInterface("/dev/i2c-0", id);
    if (fd == -1) {
        printf("i2c failed");
    }
    return fd;
}

uint8_t virtualReadRegister(uint8_t virtualAddr, int fd){
    printf("virtualReadRegister\n");
    uint8_t  status;
    //Do a prelim check of the read register
    status =  wiringPiI2CReadReg8 (fd, AS72XX_SLAVE_STATUS_REG);
    if ((status & AS72XX_SLAVE_RX_VALID) != 0){ //There is data to be read
        uint8_t incoming = wiringPiI2CReadReg8(fd, AS72XX_SLAVE_READ_REG); //Read the uint8_t but do nothing with it
    }
    printf("first read\n");
    //Wait for WRITE flag to clear
    while (1) {
        status = wiringPiI2CReadReg8(fd, AS72XX_SLAVE_STATUS_REG);
        if ((status & AS72XX_SLAVE_TX_VALID) == 0){
            break; // If TX bit is clear, it is ok to write
        }
        printf("status: %d delay%d\n",status,POLLING_DELAY);
        delay(POLLING_DELAY);
    }
    printf("write flag clear\n");
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

int main() {
    // printf() displays the string inside quotation
    printf("----------AS726X-Test----------\n");
    int fd =  wiringPiI2CSetup(DEVICE_ID);
    printf("fd is:%d\n",fd);
    if (fd == -1) {
        printf("i2c failed");
    }
    int temp = virtualReadRegister(AS726x_DEVICE_TEMP, fd);
    printf("Temperatur %dF",temp);
    return 0;
}
/*
//Read a virtual register from the AS726x
uint8_t virtualReadRegister(uint8_t virtualAddr)
{

    // Send the virtual register address (bit 7 should be 0 to indicate we are reading a register).
    writeRegister(AS72XX_SLAVE_WRITE_REG, virtualAddr);
    //Wait for READ flag to be set
    while (1)
    {
        status = readRegister(AS72XX_SLAVE_STATUS_REG);
        if ((status & AS72XX_SLAVE_RX_VALID) != 0) break; // Read data is ready.
        delay(POLLING_DELAY);
    }

    uint8_t incoming = readRegister(AS72XX_SLAVE_READ_REG);
    return (incoming);
}*/
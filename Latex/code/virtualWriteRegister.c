
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

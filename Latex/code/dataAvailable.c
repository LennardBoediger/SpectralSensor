//Checks to see if DRDY flag is set in the control setup register
//TODO: was bool test retuned 2
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
//Does a soft reset
//Give sensor at least 1000ms to reset
void softReset(int fd){
    uint8_t value = virtualReadRegister(AS726x_CONTROL_SETUP,fd); //Read
    value |= (1 << 7); //Set RST bit
    virtualWriteRegister(AS726x_CONTROL_SETUP, value, fd); //Write
}
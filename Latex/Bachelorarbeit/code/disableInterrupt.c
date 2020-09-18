//Disables the interrupt pin witch is not connected so disable it!
void disableInterrupt(int fd) {
    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_CONTROL_SETUP, fd); //Read
    value &= 0b10111111; //Clear INT bit
    virtualWriteRegister(AS726x_CONTROL_SETUP, value, fd); //Write
}
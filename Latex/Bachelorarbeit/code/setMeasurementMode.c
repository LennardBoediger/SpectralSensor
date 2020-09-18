//Sets the measurement mode
//Mode 0: Continuous reading of VBGY (7262) / STUV (7263)
//Mode 1: Continuous reading of GYOR (7262) / RTUX (7263)
//Mode 2: Continuous reading of all channels (power-on default)
//Mode 3: One-shot reading of all channels
void setMeasurementMode(uint8_t mode, int fd) {
    if (mode > 0b11) mode = 0b11;

    //Read, mask/set, write
    uint8_t value = virtualReadRegister(AS726x_CONTROL_SETUP, fd); //Read
    value &= 0b11110011; //Clear BANK bits
    value |= (mode << 2); //Set BANK bits with user's choice
    virtualWriteRegister(AS726x_CONTROL_SETUP, value, fd); //Write
}
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
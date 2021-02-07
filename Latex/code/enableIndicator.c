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
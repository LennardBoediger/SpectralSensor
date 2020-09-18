//Returns the various calibration data
float getCalibratedX(int fd) { return(getCalibratedValue(AS7261_X_CAL, fd)); } 
float getCalibratedY(int fd) { return(getCalibratedValue(AS7261_Y_CAL, fd)); } 
float getCalibratedZ(int fd) { return(getCalibratedValue(AS7261_Z_CAL, fd)); } 
float getCalibratedLUX(int fd) { return(getCalibratedValue(AS7261_LUX_CAL, fd)); } 
float getCalibratedCCT(int fd) { return(getCalibratedValue(AS7261_CCT_CAL, fd)); } 

//Given an address, read four uint8_ts and return the floating point calibrated value
float getCalibratedValue(uint8_t calAddress, int fd) {
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
float convertBytesToFloat(uint32_t myLong) {
    float myFloat;
    memcpy(&myFloat, &myLong, 4); //Copy uint8_ts into a float
    return (myFloat);
}
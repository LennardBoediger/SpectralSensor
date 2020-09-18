//Returns the temperature in C
//Pretty inaccurate: +/-8.5C //TODO: mabe include external Termometer to improve Readings
uint8_t getTemperature(int fd){
    return (virtualReadRegister(AS726x_DEVICE_TEMP, fd));
}
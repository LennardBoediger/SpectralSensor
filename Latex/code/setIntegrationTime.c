//Sets the integration value
//Give this function a uint8_t from 0 to 255.
//Time will be 2.8ms * [integration value]
void setIntegrationTime(uint8_t integrationValue, int fd) {
    virtualWriteRegister(AS726x_INT_T, integrationValue, fd); //Write
}
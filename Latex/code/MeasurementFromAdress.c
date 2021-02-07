//Calls takeMeasurements on gives I2C Adrress
void MeasurementFromAdress(int address){
    int fd =  wiringPiI2CSetup(address);
    if (fd == -1) {
        printf("i2c failed");
    }
    takeMeasurements(fd);  // takesMeasurmant Readings can now be accessed via getX(), getY(), etc
    close(fd);
}
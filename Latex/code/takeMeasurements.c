//Tells IC to take measurements and polls for data ready flag
void takeMeasurements(int fd) {
    clearDataAvailable(fd); //Clear DATA_RDY flag when using Mode 3

    //Goto mode 3 for one shot measurement of all channels
    setMeasurementMode(3, fd);

    //Wait for data to be ready
    while (dataAvailable(fd) == 0) delay(POLLING_DELAY); //Potential TODO: avoid this to get faster nearly parralel mesurments

    //Readings can now be accessed via getViolet(), getBlue(), etc
}
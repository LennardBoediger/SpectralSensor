//Test if Slave1 and 2 are detected.  Datasheet is wrong! AS72652 -> Bit 4, AS72653 -> Bit 5
uint8_t scan_AS7262(int fd) {
    uint8_t value = virtualReadRegister(AS7265X_DEV_SELECT_CONTROL, fd);
    if ((value & 0b00010000) == 1 << AS72652_ACTIVE){
        return 1;
    }
    else{
        return 0;
    }
}

uint8_t scan_AS7263(int fd) {
    uint8_t value = virtualReadRegister(AS7265X_DEV_SELECT_CONTROL, fd);
    if ((value & 0b00100000) == 1 << AS72653_ACTIVE){
        return 1;
    }
    else{
        return 0;
    }
}
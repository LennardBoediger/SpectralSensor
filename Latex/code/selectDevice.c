//Select witch AS7265X Device is used
//AS72651_id or AS72652_id or AS72653_id  
void selectDevice(uint8_t device, int fd) {
  //Set the bits 0:1. Just overwrite whatever is there because masking in the correct value doesn't work.
  virtualWriteRegister(AS7265X_DEV_SELECT_CONTROL, device, fd);
}
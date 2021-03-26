//A the 16-bit value stored in a given channel registerReturns
int getChannel(uint8_t channelRegister, int fd){
    int colorData = virtualReadRegister(channelRegister , fd) << 8; //High uint8_t
    colorData |= virtualReadRegister(channelRegister + 1, fd); //Low uint8_t
    return(colorData);
}
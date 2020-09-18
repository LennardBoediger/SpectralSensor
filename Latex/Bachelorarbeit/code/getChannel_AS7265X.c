// returns Color channel of AS7265X
// returns -1 if slave AS72651 or AS72652 is not available
int getChannel_AS7265X(int device, uint8_t channelRegister, int fd){
    selectDevice(AS72651_id, fd);   //select AS72651 to verify presence of slave sensors
    if (device == AS72651_id){
        return getChannel(channelRegister, fd);
    }
    else if(device == AS72652_id && scan_AS7262(fd)){
        selectDevice(device, fd);
        return getChannel(channelRegister, fd);
    }
    else if (device == AS72653_id && scan_AS7263(fd)){
        selectDevice(device, fd);
        return getChannel(channelRegister, fd);
    }
    return -1;
}
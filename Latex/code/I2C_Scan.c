// Scanns for sensors on all 128 possible addresses
// input pointer to to array of sensor_list struct size hast to be 128
// wirtes sensor address and type to array of sensor_list struct
void I2C_Scan(sensor_list *const s){
    //printf("test struct address in fuction%i is value %i\n",s[0].address, s[0].type );
    printf("----------I2C Scan ----------\n");
    uint8_t sensor_count = 0;
    for (int address = 0; address < 128; ++address)
    {
        int fd =  wiringPiI2CSetup(address);
        if (fd != -1){
            //try to write to some hopefully unused register(5)-> return value: 0 indicates that someone was listening 
            //this possibly writes to the Integration Value Register so make shure to properly set is before starting a measurement
            if (wiringPiI2CWriteReg8 (fd, 5, 1) == 0){ 
                int version = getVersion(fd);
                if (version == SENSORTYPE_AS7261){
                    printf("Device at: 0x%X is AS7261\n",address);
                    s[sensor_count].address = address;
                    s[sensor_count].type = SENSORTYPE_AS7261;
                    sensor_count++;
                }
                else if(version == SENSORTYPE_AS72651){
                    printf("Device at: 0x%X is AS72651",address);
                    s[sensor_count].address = address;
                    s[sensor_count].type = SENSORTYPE_AS72651;
                    sensor_count++;
                    if (scan_AS7262(fd)){
                        printf(", AS72652");
                    }
                    if (scan_AS7263(fd)){
                        printf(", AS72653");
                    }
                    printf("\n");
                }
            }
        }
        close(fd);
    }
    printf("-----------------------------\n");
}
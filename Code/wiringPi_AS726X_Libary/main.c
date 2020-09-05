#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <time.h>
#include <stdint.h>
#include "AS726X.h"
#include "influxdb.h"

void Test_address(int address);


uint64_t current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

void Test_address(int address){
    printf("----------Test address %X ----------\n",address);
    int fd =  wiringPiI2CSetup(address);
    printf("fd:%d\n",fd);
    if (fd == -1) {
        printf("i2c failed");
    }
    int version = getVersion(fd);
    if (version == 61){
        printf("AS7261\n");
    }
    //Turn Off Power LED
    disableBulb(fd);
    //Settings
    setIntegrationTime(127 ,fd);
    uint64_t measurment_time = current_timestamp();
    takeMeasurements(fd);  // takesMeasurmant Readings can now be accessed via getX(), getY(), etc
    //Save R
    int R = getR(fd);
    printf ( "getR: %d\n",R);
    writeToDatabase("R",address,measurment_time,R);

    //Save G
    int G = getG(fd);
    printf ( "getG: %d\n",G);
    writeToDatabase("G",address,measurment_time,G);

   /*
    //Save X
    int X = getX(fd);
    printf( "getX: %d\n",X);
    writeToDatabase("X",address,measurment_time,X);
    //Save Y
    int Y = getY(fd);
    printf( "getY: %d\n",Y);
    writeToDatabase("Y",address,measurment_time,Y);
    //Save Z
    int Z = getZ(fd);
    printf( "getZ: %d\n",Z);
    writeToDatabase("Z",address,measurment_time,Z);
    //Save Clear
    int Clear = getClear(fd);
    printf( "getClear: %d\n",Clear);
    writeToDatabase("Clear",address,measurment_time,Clear);
    //Save Dark
    int Dark = getDark(fd);
    printf( "getDark: %d\n",Dark);
    writeToDatabase("Dark",address,measurment_time,Dark);
    //Save NIR
    int NIR = getNIR(fd);
    printf("getNIR: %d\n",NIR);
    writeToDatabase("NIR",address,measurment_time,NIR);

    //Save Cal_X
    float Cal_X = getCalibratedX(fd);
    printf("get Cal_X: %f\n", Cal_X);
    writeToDatabase("Cal_X",address,measurment_time, Cal_X);
    //Save Cal_Y
    float Cal_Y = getCalibratedY(fd);
    printf("get Cal_Y: %f\n", Cal_Y);
    writeToDatabase("Cal_Y",address,measurment_time, Cal_Y);
    //Save Cal_Z
    float Cal_Z = getCalibratedZ(fd);
    printf("get Cal_Z: %f\n", Cal_Z);
    writeToDatabase("Cal_Z",address,measurment_time, Cal_Z);
    //Save LUX
    float Cal_LUX = getCalibratedLUX(fd);
    printf("getLUX: %f\n", Cal_LUX);
    writeToDatabase("Cal_LUX",address,measurment_time, Cal_LUX);
    //Save CCT
    float Cal_CCT = getCalibratedCCT(fd);
    printf("getCCT: %f\n", Cal_CCT);
    writeToDatabase("Cal_CCT",address,measurment_time, Cal_CCT);
    //Turn ON Power LED
    */
    enableBulb(fd);
    close(fd); //dont forget to close the fd !!!!!!!!
}

int main() {
    while(1){
        printf("----------AS726X-Test----------\n");
        Test_address(0x49);
        //Test_address(0x48);
        //sleep(1);//sleep 10s
    }
    return 0;
}


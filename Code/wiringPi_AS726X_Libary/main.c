#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "AS726X.h"
#include "influxdb.h"

void Test_address(int address);

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
    //Try to read something
    printf("Temperatur %dF\n",getTemperature(fd));
    //Settings
    setIntegrationTime(127 ,fd);
    takeMeasurements(fd);  // takesMeasurmant Readings can now be accessed via getX(), getY(), etc
    //Save X
    int X = getX(fd);
    printf( "getX: %d\n",X);
    writeToDatabase("X",address,X);
    //Save Y
    int Y = getY(fd);
    printf( "getY: %d\n",Y);
    writeToDatabase("Y",address,Y);
    //Save Z
    int Z = getZ(fd);
    printf( "getZ: %d\n",Z);
    writeToDatabase("Z",address,Z);
    //Save Clear
    int Clear = getClear(fd);
    printf( "getClear: %d\n",Clear);
    writeToDatabase("Clear",address,Clear);
    //Save Dark
    int Dark = getDark(fd);
    printf( "getDark: %d\n",Dark);
    writeToDatabase("Dark",address,Dark);
    //Save NIR
    int NIR = getNIR(fd);
    printf("getNIR: %d\n",NIR);
    writeToDatabase("NIR",address,NIR);

    //Save Cal_X
    float Cal_X = getCalibratedX(fd);
    printf("get Cal_X: %f\n", Cal_X);
    writeToDatabase("Cal_X",address, Cal_X);
    //Save Cal_Y
    float Cal_Y = getCalibratedY(fd);
    printf("get Cal_Y: %f\n", Cal_Y);
    writeToDatabase("Cal_Y",address, Cal_Y);
    //Save Cal_Z
    float Cal_Z = getCalibratedZ(fd);
    printf("get Cal_Z: %f\n", Cal_Z);
    writeToDatabase("Cal_Z",address, Cal_Z);
    //Save LUX
    float Cal_LUX = getCalibratedLUX(fd);
    printf("getLUX: %f\n", Cal_LUX);
    writeToDatabase("Cal_LUX",address,Cal_LUX);
    //Save CCT
    float Cal_CCT = getCalibratedCCT(fd);
    printf("getCCT: %f\n", Cal_CCT);
    writeToDatabase("Cal_CCT",address,Cal_CCT);

    //Turn ON Power LED
    enableBulb(fd);
    close(fd); //dont forget to close the fd !!!!!!!!
}

int main() {
    while(1){
        printf("----------AS726X-Test----------\n");
        Test_address(0x49);
        Test_address(0x48);
        sleep(1);//sleep 10s
    }
    return 0;
}


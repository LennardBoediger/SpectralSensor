#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "AS726X.h"
#include "influxdb.h"


void Test_Device(int adress){
    printf("----------Test Device %X ----------\n",adress);
    int fd =  wiringPiI2CSetup(adress);
    printf("fd:%d\n",fd);
    if (fd == -1) {
        printf("i2c failed");
    }

    disableBulb(fd);
    printf("Temperatur %dF\n",getTemperature(fd));
    int B = getBlue(fd)
    printf( "getBlue: %d\n",B);
    writeToDatabase('B',B);
    /*printf( "getGreen: %d\n",getGreen(fd));
    printf( "getYellow: %d\n",getYellow(fd));
    printf( "getOrange: %d\n",getOrange(fd));
    printf( "getRed: %d\n",getRed(fd));*/
    enableBulb(fd);


}

int main() {
    printf("----------AS726X-Test----------\n");
    Test_Device(0x49);
    Test_Device(0x48);
    return 0;
}


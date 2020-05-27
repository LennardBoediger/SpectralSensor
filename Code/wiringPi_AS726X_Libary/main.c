#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include "AS726X.h"

int main() {
    printf("----------AS726X-Test----------\n");
    int fd =  wiringPiI2CSetup(AS726X_ADDR);
    printf("fd:%d\n",fd);
    if (fd == -1) {
        printf("i2c failed");
    }
    int temp = virtualReadRegister(AS726x_DEVICE_TEMP, fd);
    printf("Temperatur %dF\n",temp);
    //enableBulb(fd);
    disableBulb(fd);
    return 0;
}

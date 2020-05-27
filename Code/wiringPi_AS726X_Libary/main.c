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

    printf("Temperatur %dF\n",getTemperature(fd));

    disableBulb(fd);
    setBulbCurrent(4,fd);
    enableBulb(fd);

    printf("dataAvailable(int fd): %d\n", dataAvailable(fd));
    return 0;
}

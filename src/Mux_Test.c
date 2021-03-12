#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

int main(){
	printf("Mux Test:\n");
    int fd =  wiringPiI2CSetup(0x70);
    if (fd == -1) {
        printf("i2c failed");
    }
    printf("i2c-connected\n");
    // Send byte 0
    wiringPiI2CWrite(fd, 0b00000001);
    printf("Done\n");
    return 0;
}

#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <time.h>
#include <stdint.h>
#include "AS726X.h"
#include "influxdb.h"


void Test_address(int address);

//returns current epoch time in ms
uint64_t current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    uint64_t milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    // printf("milliseconds: %lld\n", milliseconds);
    return milliseconds;
}

void saveAS7261Mesurements(int address, uint64_t measurment_time){
    int fd =  wiringPiI2CSetup(address);
    if (fd == -1) {
        printf("i2c failed");
    }
    // Save X
    int X = getX_CIE(fd);
    printf ( "0x%X getX: %d\n",address,X);
    writeToDatabase("X",address,measurment_time,X);
    // Save Y
    int Y = getY_CIE(fd);
    printf ( "0x%X getY: %d\n",address,Y);
    writeToDatabase("Y",address,measurment_time,Y);
    // Save Z
    int Z = getZ_CIE(fd);
    printf ( "0x%X getZ: %d\n",address,Z);
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

    close(fd);
}

void saveAS7265XMesurements(int address, uint64_t measurment_time){
    int fd =  wiringPiI2CSetup(address);
    printf("saveAS7261Mesurements fd:%o\n",fd);
    if (fd == -1) {
        printf("i2c failed");
    }
    // AS7261
    // Save R
    int R = getR(fd);
    printf ( "0x%X getR: %d\n",address,R);
    writeToDatabase("R",address,measurment_time,R);
    // Save S
    int S = getS(fd);
    printf ( "0x%X getS: %d\n",address,S);
    writeToDatabase("S",address,measurment_time,S);
    // Save T
    int T = getT(fd);
    printf ( "0x%X getT: %d\n",address,T);
    writeToDatabase("T",address,measurment_time,T);
    // Save U
    int U = getU(fd);
    printf ( "0x%X getU: %d\n",address,U);
    writeToDatabase("U",address,measurment_time,U);
    // Save V
    int V = getV(fd);
    printf ( "0x%X getV: %d\n",address,V);
    writeToDatabase("V",address,measurment_time,V);
    // Save W
    int W = getW(fd);
    printf ( "0x%X getW: %d\n",address,W);
    writeToDatabase("W",address,measurment_time,W);

    // AS7262
    // Save G
    int G = getG(fd);
    printf ( "0x%X getG: %d\n",address,G);
    writeToDatabase("G",address,measurment_time,G);
    // Save X
    int X = getX(fd);
    printf ( "0x%X getX: %d\n",address,X);
    writeToDatabase("X",address,measurment_time,X);
    // Save I
    int I = getI(fd);
    printf ( "0x%X getI: %d\n",address,I);
    writeToDatabase("I",address,measurment_time,I);
    // Save J
    int J = getJ(fd);
    printf ( "0x%X getJ: %d\n",address,J);
    writeToDatabase("J",address,measurment_time,J);
    // Save K
    int K = getK(fd);
    printf ( "0x%X getK: %d\n",address,K);
    writeToDatabase("K",address,measurment_time,K);
    // Save L
    int L = getL(fd);
    printf ( "0x%X getL: %d\n",address,L);
    writeToDatabase("L",address,measurment_time,L);

    //AS72653
    // Save A
    int A = getA(fd);
    printf ( "0x%X getA: %d\n",address,A);
    writeToDatabase("A",address,measurment_time,A);
    // Save B
    int B = getB(fd);
    printf ( "0x%X getB: %d\n",address,B);
    writeToDatabase("B",address,measurment_time,B);
    // Save C
    int C = getC(fd);
    printf ( "0x%X getC: %d\n",address,C);
    writeToDatabase("C",address,measurment_time,C);
    // Save D
    int D = getD(fd);
    printf ( "0x%X getD: %d\n",address,D);
    writeToDatabase("D",address,measurment_time,D);
    // Save E
    int E = getE(fd);
    printf ( "0x%X getE: %d\n",address,E);
    writeToDatabase("E",address,measurment_time,E);
    // Save F
    int F = getF(fd);
    printf ( "0x%X getF: %d\n",address,F);
    writeToDatabase("F",address,measurment_time,F);




    close(fd);
}

int main() {
    sensor_list s[128]; //Available Sensors are stored here
    for (int i = 0; i < 128; ++i){ //fill Sensor List with defualt value
        s[i].address = -1;
    }
    I2C_Scan(s); //Scan for Sensors
    int i=0;
    uint64_t measurment_time = current_timestamp(); // save measurment time
    while(s[i].address != -1 && i < 128){ // take Measurements from all available Sensors
            MeasurementFromAdress(s[i].address);
            i++;
    }
    uint64_t measurment_duration = current_timestamp()-measurment_time; // save measurment time
    printf("measurment_duration: %lums\n", measurment_duration);
    i=0;
    while(s[i].address != -1 && i < 128){ // Save all Measurements to influxdb
        if (s[i].type == SENSORTYPE_AS7261){
            saveAS7261Mesurements(s[i].address , measurment_time);  //Save AS7261
        } 
        else if (s[i].type == SENSORTYPE_AS72651){
            saveAS7265XMesurements(s[i].address , measurment_time); //Save AS7265X
        }
         i++;
    }

    return 0;
}


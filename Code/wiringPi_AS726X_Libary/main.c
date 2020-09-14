#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <time.h>
#include <stdint.h>
#include "AS726X.h"
#include "influxdb.h"

struct measurmentSettings{
    uint8_t integrationValue; //Give this function a uint8_t from 0 to 255. //Time will be 2.8ms * [integration value]
    uint8_t gain; //Gain 0: 1x (power-on default) //Gain 1: 3.7x //Gain 2: 16x //Gain 3: 64x
    uint16_t MesuremntIntervall; //in min
};
typedef struct measurmentSettings measurmentSettings;

//returns current epoch time in ms
uint64_t current_timestamp() {
    struct timeval te; 
    gettimeofday(&te, NULL); // get current time
    uint64_t milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    return milliseconds;
}

void saveAS7261Mesurements(int address, uint64_t measurment_time){
    int fd =  wiringPiI2CSetup(address);
    if (fd == -1) {
        printf("i2c failed");
    }
    // Save X
    int X = getX_CIE(fd);
    printf ( "0x%X getX: %d",address,X);
    writeToDatabase("X",address,measurment_time,X);
    // Save Y
    int Y = getY_CIE(fd);
    printf ( "0x%X getY: %d",address,Y);
    writeToDatabase("Y",address,measurment_time,Y);
    // Save Z
    int Z = getZ_CIE(fd);
    printf ( "0x%X getZ: %d",address,Z);
    writeToDatabase("Z",address,measurment_time,Z);

    //Save Clear
    int Clear = getClear(fd);
    printf( "0x%X getClear: %d",address,Clear);
    writeToDatabase("Clear",address,measurment_time,Clear);
    //Save Dark
    int Dark = getDark(fd);
    printf( "0x%X getDark: %d",address,Dark);
    writeToDatabase("Dark",address,measurment_time,Dark);
    //Save NIR
    int NIR = getNIR(fd);
    printf("0x%X getNIR: %d",address,NIR);
    writeToDatabase("NIR",address,measurment_time,NIR);

    //Save Cal_X
    float Cal_X = getCalibratedX(fd);
    printf("0x%X get Cal_X: %f",address , Cal_X);
    writeToDatabase("Cal_X",address,measurment_time, Cal_X);
    //Save Cal_Y
    float Cal_Y = getCalibratedY(fd);
    printf("0x%X get Cal_Y: %f",address , Cal_Y);
    writeToDatabase("Cal_Y",address,measurment_time, Cal_Y);
    //Save Cal_Z
    float Cal_Z = getCalibratedZ(fd);
    printf("0x%X get Cal_Z: %f",address , Cal_Z);
    writeToDatabase("Cal_Z",address,measurment_time, Cal_Z);
    //Save LUX
    float Cal_LUX = getCalibratedLUX(fd);
    printf("0x%X getLUX: %f",address , Cal_LUX);
    writeToDatabase("Cal_LUX",address,measurment_time, Cal_LUX);
    //Save CCT
    float Cal_CCT = getCalibratedCCT(fd);
    printf("0x%X getCCT: %f",address , Cal_CCT);
    writeToDatabase("Cal_CCT",address,measurment_time, Cal_CCT);

    close(fd);
}

void saveAS7265XMesurements(int address, uint64_t measurment_time){
    int fd =  wiringPiI2CSetup(address);
    if (fd == -1) {
        printf("i2c failed");
    }
    // AS7261
    // Save R
    int R = getR(fd);
    printf ( "0x%X getR: %d",address,R);
    writeToDatabase("R",address,measurment_time,R);
    // Save S
    int S = getS(fd);
    printf ( "0x%X getS: %d",address,S);
    writeToDatabase("S",address,measurment_time,S);
    // Save T
    int T = getT(fd);
    printf ( "0x%X getT: %d",address,T);
    writeToDatabase("T",address,measurment_time,T);
    // Save U
    int U = getU(fd);
    printf ( "0x%X getU: %d",address,U);
    writeToDatabase("U",address,measurment_time,U);
    // Save V
    int V = getV(fd);
    printf ( "0x%X getV: %d",address,V);
    writeToDatabase("V",address,measurment_time,V);
    // Save W
    int W = getW(fd);
    printf ( "0x%X getW: %d",address,W);
    writeToDatabase("W",address,measurment_time,W);

    // AS7262
    // Save G
    int G = getG(fd);
    printf ( "0x%X getG: %d",address,G);
    writeToDatabase("G",address,measurment_time,G);
    // Save X
    int X = getX(fd);
    printf ( "0x%X getX: %d",address,X);
    writeToDatabase("X",address,measurment_time,X);
    // Save I
    int I = getI(fd);
    printf ( "0x%X getI: %d",address,I);
    writeToDatabase("I",address,measurment_time,I);
    // Save J
    int J = getJ(fd);
    printf ( "0x%X getJ: %d",address,J);
    writeToDatabase("J",address,measurment_time,J);
    // Save K
    int K = getK(fd);
    printf ( "0x%X getK: %d",address,K);
    writeToDatabase("K",address,measurment_time,K);
    // Save L
    int L = getL(fd);
    printf ( "0x%X getL: %d",address,L);
    writeToDatabase("L",address,measurment_time,L);

    //AS72653
    // Save A
    int A = getA(fd);
    printf ( "0x%X getA: %d",address,A);
    writeToDatabase("A",address,measurment_time,A);
    // Save B
    int B = getB(fd);
    printf ( "0x%X getB: %d",address,B);
    writeToDatabase("B",address,measurment_time,B);
    // Save C
    int C = getC(fd);
    printf ( "0x%X getC: %d",address,C);
    writeToDatabase("C",address,measurment_time,C);
    // Save D
    int D = getD(fd);
    printf ( "0x%X getD: %d",address,D);
    writeToDatabase("D",address,measurment_time,D);
    // Save E
    int E = getE(fd);
    printf ( "0x%X getE: %d",address,E);
    writeToDatabase("E",address,measurment_time,E);
    // Save F
    int F = getF(fd);
    printf ( "0x%X getF: %d",address,F);
    writeToDatabase("F",address,measurment_time,F);




    close(fd);
}
uint64_t MeasurementFromAllAdresses(sensor_list *const s){
    int i=0;
    uint64_t measurment_time = current_timestamp(); // save measurment time
    while(s[i].address != -1 && i < 128){ // take Measurements from all available Sensors
            MeasurementFromAdress(s[i].address);
            i++;
    }
    uint64_t measurment_duration = current_timestamp()-measurment_time; // save measurment time
    printf("measurment_duration: %lums\n", measurment_duration);
    return measurment_time;
}

void settings(){
    printf("I'm sorry Dave, I'm afraid I can't do that\n");
   // void disableInterrupt(int fd);
   // setIntegrationTime(uint8_t integrationValue, int fd);
   // setGain(uint8_t gain, int fd); 
    //TODO setMesuremntIntervall();
}
void welcomeMessage(){
    printf("\n");
    printf("NdsyNyssssssssssssssssssssssssssssssssssssssdNhhNm\n");
    printf("M:  h+            ```````` ` ```            d+  sN\n");
    printf("Myos+            .ysyodsyhohossh`           -ssshN\n");
    printf("M.               .ooosooosssosoo`               -M\n");
    printf("M.               ./soosshhsosso/`               -M\n");
    printf("M.               .+ssossyyooosso`               -M\n");
    printf("M.               ./oso++++++oso+`               -N\n");
    printf("M.               .ooooo++++ooo+o`               -M\n");
    printf("M.  `...``.``.`..:soo/s/+o/o+oos-..`.``.`..`..  -M\n");
    printf("M.  osos+osoos+ooh              yso+o+os+ssooh  -M\n");
    printf("M.  /ssossssooooos     .--.     ooooooososossy  -M\n");
    printf("M.  /hossyhs++/+oo    :h//sy    +oo/++shysooys  -N\n");
    printf("M.  +hoosyds++/+oo    /d--od    os+/++sdhssohy  -M\n");
    printf("M.  +sossssoo+oooo     -//:`    +oooooossossso  -M\n");
    printf("M.  oyosoooooo+ooh              yooosossoooooh  -M\n");
    printf("M.  .:.:-.-..-.-:://+//-/::+:+++/::-:.-:.--::/  -M\n");
    printf("M.                s+ooo++++oooos`               -N\n");
    printf("M.                /oss++++++oso/`               -N\n");
    printf("M.                +ssoooyysoosso`               -M\n");
    printf("M.                /ossoshdyssos+`               -M\n");
    printf("M.                ++sosssooosooo`               -N\n");
    printf("Myss+`            yossyohsshoysy`           .sssyN\n");
    printf("M/  yo            ``````````````            h+  oN\n");
    printf("MmyyMhssssssssssssssssssssssssssssssssssssssdNhyNN\n");
    printf("Welcome to Spectral Sensor V0.8\n");
    printf("Exit by pressing ctr+c\n");
    printf("Enter [DeviceIP]:3000 into your browser to access the measurement data.\n");
}

int main() {
    welcomeMessage();
    char userSettingResponse;
    measurmentSettings Settings; //measurmentSettings are stored here 
    Settings.integrationValue = 128; 
    Settings.gain = 1;
    Settings.MesuremntIntervall = 1; 

    sensor_list s[128]; //Available Sensors are stored here
    for (int i = 0; i < 128; ++i){ //fill Sensor List with defualt value
        s[i].address = -1;
    }
    while(1){
        I2C_Scan(s); //Scan for Sensors
        printf("Please check if all expected devices are available.\n");
        printf("----------Settings-----------\n");
        printf("Integration Value: %hhu * 2.8ms = Integration Time\n",Settings.integrationValue);
        printf("Gain: %hhu\n", Settings.gain);
        printf("Mesuremnt Intervall: %u min\n", Settings.MesuremntIntervall);
        printf("-----------------------------\n");
        printf("Are The Settings Correct? Type y to continue, n to change Settings \n");
        scanf(" %1c", &userSettingResponse);
        //userSettingResponse = getchar();
        if (userSettingResponse == 'N' || userSettingResponse == 'n'){
            printf("\n\n\n\n");
            settings();
        }
        else if (userSettingResponse == 'Y' || userSettingResponse == 'y'){
            printf("--Starting Measurment Cycle--\n");
            break;
        }
        else{
            printf("Error\n\n\n\n");
        }
    }

    uint64_t measurment_time = MeasurementFromAllAdresses(s);
    uint64_t read_time = current_timestamp(s);
    int i=0;
    while(s[i].address != -1 && i < 128){ // Save all Measurements to influxdb
        if (s[i].type == SENSORTYPE_AS7261){
            saveAS7261Mesurements(s[i].address , measurment_time);  //Save AS7261
        } 
        else if (s[i].type == SENSORTYPE_AS72651){
            saveAS7265XMesurements(s[i].address , measurment_time); //Save AS7265X
        }
         i++;
    }
    uint64_t read_duration = current_timestamp(s)-read_time;
    printf("read_duration: %lu\n",read_duration );

    return 0;
}


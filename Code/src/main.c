#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <sys/time.h> 
#include <stdint.h>
#include "../lib/wiringPi_AS726X_Libary/AS726X.h"
#include "../lib/influxDB_http_Libary/influxdb.h"
#include "welcome.h"

struct measurmentSettings{
    uint8_t integrationValue; //Give this function a uint8_t from 0 to 255. //Time will be 2.8ms * [integration value]
    uint8_t gain; //Gain 0: 1x (power-on default) //Gain 1: 3.7x //Gain 2: 16x //Gain 3: 64x
    uint16_t MesuremntIntervall; //in min
};
typedef struct measurmentSettings measurmentSettings;

//returns current epoch time in ms
uint64_t current_timestamp() {
    struct timeval te;        // struct with microsecond precision
    gettimeofday(&te, NULL); // get current time
    uint64_t milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    return milliseconds;
}

//Delays for given amount of minutes
void delayMesuremntMin(uint16_t MesuremntIntervall){
    time_t current_time;                        // Curren time for comparison
    time_t next_mesuremnt;                      // Calculated time for next measurment
    struct tm * timeinfo;                       // time struct for print
    time (&next_mesuremnt);                     // save current time to next_mesuremnt
    next_mesuremnt += MesuremntIntervall * 60;  // add MesuremntIntervall to current time 
    timeinfo = localtime ( &next_mesuremnt );   // convert next_mesuremnt time to tm struct 
    printf ( "Next Mesuremnt: %s", asctime (timeinfo) );
    do{                                         // wait until next measurement time is reached
    time (&current_time);                       // save current time to current_time
    } while(next_mesuremnt > current_time);     

}


void saveAS7261Mesurements(int address, uint64_t measurment_time){
    int fd =  wiringPiI2CSetup(address);
    if (fd == -1) {
        printf("i2c failed");
    }
    // Save X
    int X = getX(fd);
    printf ( "0x%X getX: %d",address,X);
    writeToDatabase("X",address,measurment_time,X);
    // Save Y
    int Y = getY(fd);
    printf ( "0x%X getY: %d",address,Y);
    writeToDatabase("Y",address,measurment_time,Y);
    // Save Z
    int Z = getZ(fd);
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
    uint32_t Cal_LUX = getCalibratedLUX(fd);
    printf("0x%X getLUX: %lu",address , (unsigned long)Cal_LUX);
    writeToDatabase("Cal_LUX",address,measurment_time, Cal_LUX);
    //Save CCT
    uint32_t Cal_CCT = getCalibratedCCT(fd);
    printf("0x%X getCCT: %lu",address , (unsigned long)Cal_CCT);
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
    // Save H
    int H = getH(fd);
    printf ( "0x%X getH: %d",address,H);
    writeToDatabase("H",address,measurment_time,H);
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

    // Save Cal_R
    float Cal_R = getCalibratedR(fd);
    printf ( "0x%X getCal_R: %f",address,Cal_R);
    writeToDatabase("Cal_R",address,measurment_time,Cal_R);
    // Save Cal_S
    float Cal_S = getCalibratedS(fd);
    printf ( "0x%X getCal_S: %f",address,Cal_S);
    writeToDatabase("Cal_S",address,measurment_time,Cal_S);
    // Save Cal_T
    float Cal_T = getCalibratedT(fd);
    printf ( "0x%X getCal_T: %f",address,Cal_T);
    writeToDatabase("Cal_T",address,measurment_time,Cal_T);
    // Save Cal_U
    float Cal_U = getCalibratedU(fd);
    printf ( "0x%X getCal_U: %f",address,Cal_U);
    writeToDatabase("Cal_U",address,measurment_time,Cal_U);
    // Save Cal_V
    float Cal_V = getCalibratedV(fd);
    printf ( "0x%X getCal_V: %f",address,Cal_V);
    writeToDatabase("Cal_V",address,measurment_time,Cal_V);
    // Save Cal_W
    float Cal_W = getCalibratedW(fd);
    printf ( "0x%X getCal_W: %f",address,Cal_W);
    writeToDatabase("Cal_W",address,measurment_time,Cal_W);
    // Save Cal_G
    float Cal_G = getCalibratedG(fd);
    printf ( "0x%X getCal_G: %f",address,Cal_G);
    writeToDatabase("Cal_G",address,measurment_time,Cal_G);
    // Save Cal_X
    float Cal_H = getCalibratedH(fd);
    printf ( "0x%X getCal_H: %f",address,Cal_H);
    writeToDatabase("Cal_H",address,measurment_time,Cal_H);
    // Save Cal_I
    float Cal_I = getCalibratedI(fd);
    printf ( "0x%X getCal_I: %f",address,Cal_I);
    writeToDatabase("Cal_I",address,measurment_time,Cal_I);
    // Save Cal_J
    float Cal_J = getCalibratedJ(fd);
    printf ( "0x%X getCal_J: %f",address,Cal_J);
    writeToDatabase("Cal_J",address,measurment_time,Cal_J);
    // Save Cal_K
    float Cal_K = getCalibratedK(fd);
    printf ( "0x%X getCal_K: %f",address,Cal_K);
    writeToDatabase("Cal_K",address,measurment_time,Cal_K);
    // Save Cal_L
    float Cal_L = getCalibratedL(fd);
    printf ( "0x%X getCal_L: %f",address,Cal_L);
    writeToDatabase("Cal_L",address,measurment_time,Cal_L);
    // Save Cal_A
    float Cal_A = getCalibratedA(fd);
    printf ( "0x%X getCal_A: %f",address,Cal_A);
    writeToDatabase("Cal_A",address,measurment_time,Cal_A);
    // Save Cal_B
    float Cal_B = getCalibratedB(fd);
    printf ( "0x%X getCal_B: %f",address,Cal_B);
    writeToDatabase("Cal_B",address,measurment_time,Cal_B);
    // Save Cal_C
    float Cal_C = getCalibratedC(fd);
    printf ( "0x%X getCal_C: %f",address,Cal_C);
    writeToDatabase("Cal_C",address,measurment_time,Cal_C);
    // Save Cal_D
    float Cal_D = getCalibratedD(fd);
    printf ( "0x%X getCal_D: %f",address,Cal_D);
    writeToDatabase("Cal_D",address,measurment_time,Cal_D);
    // Save Cal_E
    float Cal_E = getCalibratedE(fd);
    printf ( "0x%X getCal_E: %f",address,Cal_E);
    writeToDatabase("Cal_E",address,measurment_time,Cal_E);
    // Save Cal_F
    float Cal_F = getCalibratedF(fd);
    printf ( "0x%X getCal_F: %f",address,Cal_F);
    writeToDatabase("Cal_F",address,measurment_time,Cal_F);



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
    printf("Measurment Duration: %lu ms\n", measurment_duration);
    return measurment_time;
}

void settings(int address, uint8_t integrationValue, uint8_t gain){
    int fd =  wiringPiI2CSetup(address);
    if (fd == -1) {
        printf("i2c failed");
    }
    disableInterrupt(fd);
    setIntegrationTime(integrationValue, fd);
    setGain(gain, fd); 
    close(fd);
}

void changeSettings(measurmentSettings *Settings){
    printf("\n\n\n\n\n");
    char tmp[10];    //define teporary input storage

    do{
    printf("Set Integration Value [0:255]:\n");
    fgets(tmp,10,stdin);
    } while(!(atoi(tmp)>=0 && atoi(tmp)<=255));
    Settings->integrationValue = atoi(tmp);

    do{
    printf("Set Gain [0:3] 0->1x 1->3.7x 2->16x 3->64x:\n");
    fgets(tmp,10,stdin);
    } while(!(atoi(tmp)>=0 && atoi(tmp)<=3));
    Settings->gain = atoi(tmp);

    do{
    printf("Set Mesuremnt Intervall in Minutes [1:65535]\n");
    fgets(tmp,10,stdin);
    } while(!(atoi(tmp)>=1 && atoi(tmp)<=65535));
    Settings->MesuremntIntervall = atoi(tmp); 

    printf("\n\n\n\n\n");

}


int main() {
    welcomeMessage();
    char userSettingResponse[4];
    measurmentSettings Settings; //measurmentSettings are stored here 
    //TODO change default values to be read from config file
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
        fgets(userSettingResponse,4,stdin);
        //userSettingResponse = getchar();
        if (userSettingResponse[0] == 'N' || userSettingResponse[0] == 'n'){
            changeSettings(&Settings);
        }
        else if (userSettingResponse[0] == 'Y' || userSettingResponse[0] == 'y'){
            printf("--Starting Measurment Cycle--\n");
            break;
        }
        else{
            printf("Error\n\n\n\n");
        }
    }
    //apply settings
    for (int i = 0; s[i].address != -1 && i < 128; ++i){
        settings(s[i].address, Settings.integrationValue, Settings.gain);
    }
    while(1){
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
        printf("Read Duration: %lu ms\n",read_duration );
        printf("-----------------------------\n");
        delayMesuremntMin(Settings.MesuremntIntervall);
    }
    return 0;
}


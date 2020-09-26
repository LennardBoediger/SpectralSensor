#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <sys/time.h> 
#include <stdint.h>
#include <math.h>
#include "../lib/wiringPi_AS726X_Libary/AS726X.h"
#include "../lib/influxDB_http_Libary/influxdb.h"
#include "../lib/libcsv/src/csv.h"
#include "welcome.h"

#define AUTOGAIN 4

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

void readCalibrationValues(sensor_list *const s){
    char file_name[][30] =
    { "config/calibrationX.csv",
      "config/calibrationY.csv",
      "config/calibrationZ.csv"
    };

    csv_t *csv;
    csv_read_t ret;

    for (int channel_i = 0; channel_i < 3; ++channel_i){

        csv = csv_init();
        if (csv == NULL) {
           printf("Out of memory\n");
           exit(1);
        }

        if (!csv_open_file(csv, file_name[channel_i], 0)) {
           printf("Error opening file: %s\n", csv_error(csv));
        }
        else {
           while (1) {
              ret = csv_read(csv);

              if (ret == CSV_READ_ERROR) {
                 printf("Read error: %s\n", csv_error(csv));
                 break;
              }

              if (ret == CSV_READ_EOF) {
                 break;
              }
                for (int i = 0; s[i].address != -1 && i < 128; ++i){    // going through every device
                    if (s[i].type == SENSORTYPE_AS7261){
                        if (atoi(csv_get(csv, 0))==s[i].address){
                            for (int gain_i = 0; gain_i < 4; ++gain_i){     // going through every gain possibility
                                switch (channel_i){
                                    case 0:
                                        s[i].calibration_factor_AS7261[gain_i].X = atoi(csv_get(csv, gain_i+1));
                                        break;
                                    case 1:
                                        s[i].calibration_factor_AS7261[gain_i].Y = atoi(csv_get(csv, gain_i+1));
                                        break;
                                    case 2:
                                        s[i].calibration_factor_AS7261[gain_i].Z = atoi(csv_get(csv, gain_i+1));
                                        break;

                                }
                            }
                        }
                    }
                }
           }

           csv_close(csv);
        }
        csv_free(csv);
    }
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
void printTime(){
    time_t current_time;
    struct tm * timeinfo;  
    time (&current_time); 
    timeinfo = localtime ( &current_time);
    printf ( "System Time: %s\n", asctime (timeinfo));
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
    printf("0x%X Changed Gain to %d\n",address,gain );
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
    printf("Set Gain [0:3] 0->1x 1->3.7x 2->16x 3->64x 4->Auto Gain:\n");
    fgets(tmp,10,stdin);
    } while(!(atoi(tmp)>=0 && atoi(tmp)<=4));
    Settings->gain = atoi(tmp);

    do{
    printf("Set Mesuremnt Intervall in Minutes [1:65535]\n");
    fgets(tmp,10,stdin);
    } while(!(atoi(tmp)>=1 && atoi(tmp)<=65535));
    Settings->MesuremntIntervall = atoi(tmp); 

    printf("\n\n\n\n\n");

}

uint32_t matchValueToMaxGain(int used_gain, uint32_t value){
    switch(used_gain){
        case 0:
            value *= 64;
            break;
        case 1:
            value = (uint32_t)(round((float)value * 17.2972972973));
            break;
        case 2:
            value *= 4;
            break;
        default:
            break;
    }
    return value;
    
}

//save the biggest (gain dependet)value that is not clipping
AS7261_channel cleanAS7261Data(AS7261_channel *const AS7261_measurement){
    AS7261_channel used_gain;
    used_gain.X = 0;
    for (int i = 1; i < 4; ++i){
        if (AS7261_measurement[0].X < AS7261_measurement[i].X && AS7261_measurement[i].X < 65500){
            AS7261_measurement[0].X = AS7261_measurement[i].X;
            used_gain.X = i;
        }
    }
    AS7261_measurement[0].X = matchValueToMaxGain(used_gain.X , AS7261_measurement[0].X);

    used_gain.Y = 0;
    for (int i = 1; i < 4; ++i){
        if (AS7261_measurement[0].Y < AS7261_measurement[i].Y && AS7261_measurement[i].Y < 65500){
            AS7261_measurement[0].Y = AS7261_measurement[i].Y;
            used_gain.Y = i;
        }
    }
    AS7261_measurement[0].Y = matchValueToMaxGain(used_gain.Y , AS7261_measurement[0].Y);

    used_gain.Z = 0;
    for (int i = 1; i < 4; ++i){
        if (AS7261_measurement[0].Z < AS7261_measurement[i].Z && AS7261_measurement[i].Z < 65500){
            AS7261_measurement[0].Z = AS7261_measurement[i].Z;
            used_gain.Z = i;
        }
    }
    AS7261_measurement[0].Z = matchValueToMaxGain(used_gain.Z , AS7261_measurement[0].Z);
    return used_gain;
}

AS7261_channel getAS7261Measurement(int address){
    AS7261_channel store_data;
    int fd =  wiringPiI2CSetup(address);
    if (fd == -1) {
        printf("i2c failed");
    }
    // Save X
    store_data.X = getX(fd);
    printf ( "0x%X getX: %d\n",address,store_data.X);
    // Save Y
    store_data.Y = getY(fd);
    printf ( "0x%X getY: %d\n",address,store_data.Y);
    // Save Z
    store_data.Z = getZ(fd);
    printf ( "0x%X getZ: %d\n",address,store_data.Z);

    close(fd);
    return store_data;

}

void saveAS7261Measurement(int address,AS7261_channel values, uint64_t measurement_time){
    // Save X
    printf ( "0x%X saveX: %d",address,values.X);
    writeToDatabase("X",address,measurement_time,values.X);
    // Save Y
    printf ( "0x%X saveY: %d",address,values.Y);
    writeToDatabase("Y",address,measurement_time,values.Y);
    // Save Z
    printf ( "0x%X saveZ: %d",address,values.Z);
    writeToDatabase("Z",address,measurement_time,values.Z);
}

void calibrateValuesAS7261(AS7261_channel used_gain, AS7261_channel *const AS7261_measurement){

printf("calibrate X: %d Used Gain: %d \n", AS7261_measurement[0].X, used_gain.X);
}

void autoGainMeasurementAS7261(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue){
    AS7261_channel AS7261_measurement[4];
    for (int i = 0; s[i].address != -1 && i < 128; ++i){    // going through every device
        if (s[i].type == SENSORTYPE_AS7261){                // only use AS7261 devices
            for (int gain_i = 0; gain_i < 4; ++gain_i){         // going through every gain
                settings(s[i].address, integrationValue, gain_i); // apply settings integrationValue is fix gain is 0-3
                MeasurementFromAdress(s[i].address);            // initiate measurement from current adress
                AS7261_measurement[gain_i] = getAS7261Measurement(s[i].address);    // get data and save to array
            }
            AS7261_channel used_gain = cleanAS7261Data(AS7261_measurement);   // write clean values to AS7261_measurement[0] and store used gains
            calibrateValuesAS7261(used_gain, AS7261_measurement);
            saveAS7261Measurement(s[i].address ,AS7261_measurement[0], measurement_time); // write clean values to database
        }
    }
}

void manualGainMeasurementAS7261(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue, uint8_t gain){
    AS7261_channel AS7261_measurement;
    for (int i = 0; s[i].address != -1 && i < 128; ++i){    // going through every device
        if (s[i].type == SENSORTYPE_AS7261){                // only use AS7261 devices
            settings(s[i].address, integrationValue, gain); // apply settings integrationValue is fix gain is fix
            MeasurementFromAdress(s[i].address);            // initiate measurement from current adress
            AS7261_measurement = getAS7261Measurement(s[i].address);    // get data and save
            saveAS7261Measurement(s[i].address ,AS7261_measurement, measurement_time); // write values to database
        }
    }

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

int main() {

    welcomeMessage();
    printTime();
    char userSettingResponse[4];
    measurmentSettings Settings; //measurmentSettings are stored here 
    //Default Values
    Settings.integrationValue = 128; 
    Settings.gain = 1;
    Settings.MesuremntIntervall = 1;

    sensor_list s[128]; //Available Sensors are stored here
    for (int i = 0; i < 128; ++i){ //fill Sensor List with defualt value
        s[i].address = -1;
    }
    while(1){
        I2C_Scan(s); //Scan for Sensors
        readCalibrationValues(s);
        for (int i = 0; s[i].address != -1 && i < 128; ++i){    // going through every device
            printf("Device %d\n", s[i].address);
            if (s[i].type == SENSORTYPE_AS7261){
                    printf("X Calibration Factor: G0:%d, G1:%d, G2:%d, G3,%d\n", s[i].calibration_factor_AS7261[0].X, s[i].calibration_factor_AS7261[1].X, s[i].calibration_factor_AS7261[2].X, s[i].calibration_factor_AS7261[3].X);
                    printf("Y Calibration Factor: G0:%d, G1:%d, G2:%d, G3,%d\n", s[i].calibration_factor_AS7261[0].Y, s[i].calibration_factor_AS7261[1].Y, s[i].calibration_factor_AS7261[2].Y, s[i].calibration_factor_AS7261[3].Y);
                    printf("Z Calibration Factor: G0:%d, G1:%d, G2:%d, G3,%d\n", s[i].calibration_factor_AS7261[0].Z, s[i].calibration_factor_AS7261[1].Z, s[i].calibration_factor_AS7261[2].Z, s[i].calibration_factor_AS7261[3].Z);
            }
            // else if(s[i].type == SENSORTYPE_AS72651){
            //     printf("Calibration Factor: G0:%d, G1:%d, G2:%d, G3,%d\n", s[0].calibration_factor_AS7261[gain_i].X, s[1].calibration_factor_AS7261[gain_i].X, s[2].calibration_factor_AS7261[gain_i].X, s[3].calibration_factor_AS7261[gain_i].X);
            //     printf("Calibration Factor: G0:%d, G1:%d, G2:%d, G3,%d\n", s[0].calibration_factor_AS7261[gain_i].Y, s[1].calibration_factor_AS7261[gain_i].Y, s[2].calibration_factor_AS7261[gain_i].Y, s[3].calibration_factor_AS7261[gain_i].Y);
            //     printf("Calibration Factor: G0:%d, G1:%d, G2:%d, G3,%d\n", s[0].calibration_factor_AS7261[gain_i].Z, s[1].calibration_factor_AS7261[gain_i].Z, s[2].calibration_factor_AS7261[gain_i].Z, s[3].calibration_factor_AS7261[gain_i].Z);

            // }
        }

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



    while(1){
        uint64_t measurement_time = current_timestamp(s);       // save mesuremnt time
        if (Settings.gain == AUTOGAIN){
            autoGainMeasurementAS7261(measurement_time, s, Settings.integrationValue);
            //autoGainMeasurementAS7265X(measurement_time, s, Settings.integrationValue);
        }
        else{
            manualGainMeasurementAS7261(measurement_time, s, Settings.integrationValue, Settings.gain);
            //manualGainMeasurementAS7265X(measurement_time, s, Settings.integrationValue, Settings.gain);
        }
            uint64_t measurement_duration = current_timestamp(s)-measurement_time;
            printf("Measurement duration: %lu ms\n",measurement_duration);
            printf("-----------------------------\n");
            delayMesuremntMin(Settings.MesuremntIntervall);
    }
    return 0;
}


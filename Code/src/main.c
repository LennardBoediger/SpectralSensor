#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <sys/time.h> 
#include <math.h>
#include "../lib/wiringPi_AS726X_Libary/AS726X.h"
#include "../lib/influxDB_http_Libary/influxdb.h"
#include "measurement.h"
#include "welcome.h"
#include "../default_values.h"

#define AUTOGAIN 4

struct measurmentSettings{
    uint8_t integrationValue;       //Give this function a uint8_t from 0 to 255. //Time will be 2.8ms * [integration value]
    uint8_t gain;                   //Gain 0: 1x (power-on default) //Gain 1: 3.7x //Gain 2: 16x //Gain 3: 64x
    uint16_t MesuremntIntervall;    //in min
};
typedef struct measurmentSettings measurmentSettings;

// returns current epoch time in ms
uint64_t current_timestamp() {
    struct timeval te;        // struct with microsecond precision
    gettimeofday(&te, NULL);  // get current time
    uint64_t milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    return milliseconds;
}

// Delays for given amount of minutes
void delayMesuremntMin(uint16_t MesuremntIntervall){
    time_t current_time;                        // Curren time for comparison
    time_t next_mesuremnt;                      // Calculated time for next Mesuremnt
    struct tm * next_mesuremnt_tm;                       // struct fot next Mesuremnt calculation
    time (&next_mesuremnt);                     // save current time to next_mesuremnt
    next_mesuremnt_tm = localtime ( &next_mesuremnt );   // convert next_mesuremnt time to tm struct 
    next_mesuremnt_tm->tm_min += 1+MesuremntIntervall;       // go to next min and add Intervall
    next_mesuremnt_tm->tm_sec = 0;                           // set sec to zero
    mktime(next_mesuremnt_tm);                          //normalize struct (for example min: 61->1)
    next_mesuremnt = mktime(next_mesuremnt_tm);           // convert to time_t

    printf ("Next Mesuremnt: %s", asctime (next_mesuremnt_tm));
    do{                                         // wait until next measurement time is reached
    time (&current_time);                       // save current time to current_time
    } while(next_mesuremnt > current_time);     
}


// Print the current Time
void printTime(){
    time_t current_time;
    struct tm * timeinfo;  
    time (&current_time); 
    timeinfo = localtime ( &current_time);
    printf ( "System Time: %s\n", asctime (timeinfo));
}

// Write settings to data structure at system start
// pointer to measurmentSettings data structure  
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

int main() {

    welcomeMessage();
    printTime();
    char userSettingResponse[4];
    measurmentSettings Settings; //measurmentSettings are stored here 
    //Default Values located in default_values.h
    Settings.integrationValue = DEFAULT_INTEGRATION_VALUE; 
    Settings.gain = DEFAULT_GAIN;
    Settings.MesuremntIntervall = DEFAULT_Mesuremnt_Intervall;

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

    while(1){
        uint64_t measurement_time = current_timestamp(s);       // save mesuremnt time
        if (Settings.gain == AUTOGAIN){
            autoGainMeasurementAS7261(measurement_time, s, Settings.integrationValue);
            autoGainMeasurementAS7265X(measurement_time, s, Settings.integrationValue);
        }
        else{
            manualGainMeasurementAS7261(measurement_time, s, Settings.integrationValue, Settings.gain);
            //manualGainMeasurementAS7265X(measurement_time, s, Settings.integrationValue, Settings.gain);
        }
            uint64_t measurement_duration = current_timestamp(s)-measurement_time;
            printf("Measurement duration: %llu ms\n",measurement_duration);
            printf("-----------------------------\n");
            delayMesuremntMin(Settings.MesuremntIntervall);
    }
    return 0;
}


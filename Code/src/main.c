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
    uint8_t integrationValue;       // Value from 0-255 -> Integration Time will be 2.8ms * [integration value] 
    uint8_t gain;                   // Gain 0: 1x (power-on default) //Gain 1: 3.7x //Gain 2: 16x //Gain 3: 64x
    uint16_t measurementIntervall;  // Interval of the measurement, the measurement always takes place at the second 0
};
typedef struct measurmentSettings measurmentSettings;

// returns the current epoch time in ms
uint64_t currentTimestamp() {
    struct timeval te;        // struct with microsecond precision
    gettimeofday(&te, NULL);  // get current time
    uint64_t milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; // calculate milliseconds
    return milliseconds;
}

// Delays for given amount of minutes
void delayMeasurementMin(uint16_t measurementIntervall){
    if (measurementIntervall >= 1){
        time_t current_time;                        			// Curren time for comparison
        time_t next_measurement;                      			// Calculated time for the next measurement
        struct tm * next_measurement_tm;              			// tm struct for next measurement calculation
        time (&next_measurement);                     			// Save current time to next_measurement
        next_measurement_tm = localtime ( &next_measurement );  // Convert next_measurement time to tm struct 
        next_measurement_tm->tm_min += measurementIntervall;    // Add Intervall to next_measurement tm struct
        next_measurement_tm->tm_sec = 0;                      	// Set sec of next_measurement tm struct to zero
        mktime(next_measurement_tm);                          	// Normalize struct (for example min: 61->1)
        next_measurement = mktime(next_measurement_tm);         // Convert next_measurement_tm  back to time_t

        printf ("Next measurement: %s", asctime (next_measurement_tm));
        do{                                         // Wait until next measurement time is reached
        time (&current_time);                       // Save current time to current_time
        } while(next_measurement > current_time);
    }
    else{
        printf("invalid waiting Time\n");
    }
}


// Print the current Time
void printTime(){
    time_t current_time;								// time_t struct for current time
    struct tm * timeinfo;  								// tm struct
    time (&current_time); 								// Save current time to current_time
    timeinfo = localtime ( &current_time);				// Convert time_t current_time to tm timeinfo 
    printf ( "System Time: %s\n", asctime (timeinfo));	// Print timeinfo
}

// Writing settings to the data structure at system startup
// This dose not write the settings to the Sensor! it just "collects" the settings to the data structure
// Input pointer to measurmentSettings data structure  
void changeSettings(measurmentSettings *Settings){
    printf("\n\n\n\n\n");
    char tmp[10];    // Define teporary user input storage

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
    printf("Set measurement Intervall in Minutes [1:65535]:\n");
    printf("(Note that by connecting more than 3 sensors a measurement can sometimes take longer than one minute)\n");
    fgets(tmp,10,stdin);
    } while(!(atoi(tmp)>=1 && atoi(tmp)<=65535));
    Settings->measurementIntervall = atoi(tmp); 

    printf("\n\n\n\n\n");
}

int main() {
    welcomeMessage();
    printTime();

    // Get default values located in default_values.h
    measurmentSettings Settings;	// Struct for measurment settings  
    Settings.integrationValue = DEFAULT_INTEGRATION_VALUE; 
    Settings.gain = DEFAULT_GAIN;
    Settings.measurementIntervall = DEFAULT_MESUREMNT_INTERVALL;

    char userSettingResponse[4];	// Buffer for user input
    sensor_list s[128]; 			// Available sensors are stored here
    for (int i = 0; i < 128; ++i){	// Fill sensor list with defualt value
        s[i].address = -1;
    }
    while(1){
        I2C_Scan(s); //Scan for Sensors
        printf("Please check if all expected devices are available.\n");
        printf("----------Settings-----------\n");
        printf("Integration Value: %hhu * 2.8ms = Integration Time\n",Settings.integrationValue);
        printf("Gain: %hhu\n", Settings.gain);
        printf("measurement Intervall: %u min\n", Settings.measurementIntervall);
        printf("-----------------------------\n");
        if (PLUG_AND_PLAY == 0){
            printf("Are The Settings Correct? Type y to continue, n to change Settings \n");
            fgets(userSettingResponse,4,stdin);	// Collect user input

            if (userSettingResponse[0] == 'N' || userSettingResponse[0] == 'n'){	// User says No - Change settings
                changeSettings(&Settings);
            }
            else if (userSettingResponse[0] == 'Y' || userSettingResponse[0] == 'y'){	// User says Yes - Start 
                printf("--Starting Measurment Cycle--\n");
                break;
            }
            else{
                printf("Error\n\n\n\n");	// User doesn't know what he is doing - Ask again
            }
        }
    }

    while(1){
        uint64_t measurement_time = currentTimestamp(s);       // Save measurement time - this will end up in the DB
        if (Settings.gain == AUTOGAIN){
            if (USE_AS7261){
                autoGainMeasurementAS7261(measurement_time, s, Settings.integrationValue);
            }
            if (USE_AS7265X){
                autoGainMeasurementAS7265X(measurement_time, s, Settings.integrationValue);
            }
        }
        else{
            if (USE_AS7261){
                fixedGainMeasurementAS7261(measurement_time, s, Settings.integrationValue, Settings.gain);
            }
            if (USE_AS7265X){
                fixedGainMeasurementAS7265X(measurement_time, s, Settings.integrationValue, Settings.gain);
            }
        }
            uint64_t measurement_duration = currentTimestamp(s)-measurement_time;	// Calculate measurement duration
            printf("Measurement duration: %llu ms\n",measurement_duration);			// Print measurement duration
            printf("-----------------------------\n");
            if (MANUAL_TRIGGER == 1){
                printf("Press enter for Next Measurement\n");
                char enter = 0;
                while (enter != '\r' && enter != '\n') {
                 enter = getchar(); 
                }
            }
            else{
                delayMeasurementMin(Settings.measurementIntervall);						// Wait for next measurement
            }
    }
    return 0;
}


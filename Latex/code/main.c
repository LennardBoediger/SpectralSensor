int main() {
    welcomeMessage();
    printTime();
    measurmentSettings Settings;	// for measurment settings
    
    // set default values located in default_values.h
    Settings.integrationValue = DEFAULT_INTEGRATION_VALUE; 
    Settings.gain = DEFAULT_GAIN;
    Settings.measurementIntervall = DEFAULT_measurement_Intervall;

    char userSettingResponse[4];	// for user input
    sensor_list s[128]; 			// available sensors are stored here
    for (int i = 0; i < 128; ++i){	// fill sensor list with defualt value
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
            fgets(userSettingResponse,4,stdin);	// collect user input

            if (userSettingResponse[0] == 'N' || userSettingResponse[0] == 'n'){	// User says No - change settings
                changeSettings(&Settings);
            }
            else if (userSettingResponse[0] == 'Y' || userSettingResponse[0] == 'y'){	// User says Yes - start 
                printf("--Starting Measurment Cycle--\n");
                break;
            }
            else{
                printf("Error\n\n\n\n");	// User doesn't know what he is doing - ask again
            }
        }
    }

    while(1){
        uint64_t measurement_time = currentTimestamp(s);       // save measurement time - this will end up in the DB
        if (Settings.gain == AUTOGAIN){
        	// uncomment incase you only want AS7261 or AS7265X values
            autoGainMeasurementAS7261(measurement_time, s, Settings.integrationValue);
            autoGainMeasurementAS7265X(measurement_time, s, Settings.integrationValue);
        }
        else{
        	// uncomment incase you only want AS7261 or AS7265X values
            manualGainMeasurementAS7261(measurement_time, s, Settings.integrationValue, Settings.gain);
            manualGainMeasurementAS7265X(measurement_time, s, Settings.integrationValue, Settings.gain);
        }
            uint64_t measurement_duration = currentTimestamp(s)-measurement_time;	// calculate measurement duration
            printf("Measurement duration: %llu ms\n",measurement_duration);			// print measurement duration
            printf("-----------------------------\n");
            delayMeasurementMin(Settings.measurementIntervall);						// wait for next measurement
    }
    return 0;
}
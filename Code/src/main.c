#include <stdio.h>
#include <stdint.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <sys/time.h> 
#include <stdint.h>
#include "../lib/wiringPi_AS726X_Libary/AS726X.h"
#include "../lib/influxDB_http_Libary/influxdb.h"
#include "welcome.h"

#define AUTOGAIN 4
affe affe
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


void saveAS7261Mesurements(int address,AS7261_data *const data, uint64_t measurment_time){
    for(int i = 0 ;s[i].address != -1 && i < 128; i++){ 
        if (s[i].type == SENSORTYPE_AS7261){
            int fd =  wiringPiI2CSetup(s[i].address);
            if (fd == -1) {
                printf("i2c failed");
            }

            // Save X
            printf ( "0x%X saveX: %d",address,X);
            writeToDatabase("X",address,measurment_time,data[].X);
            // Save Y
            int Y = getY(fd);
            printf ( "0x%X saveY: %d",address,Y);
            writeToDatabase("Y",address,measurment_time,data.Y);
            // Save Z
            int Z = getZ(fd);
            printf ( "0x%X saveZ: %d",saddress,Z);
            writeToDatabase("Z",address,measurment_time,data.Z);

            //Save Clear
            int Clear = getClear(fd);
            printf( "0x%X saveClear: %d",address,Clear);
            writeToDatabase("Clear",address,measurment_time,data.Clear);
            //Save Dark
            int Dark = getDark(fd);
            printf( "0x%X saveDark: %d",address,Dark);
            writeToDatabase("Dark",address,measurment_time,data.Dark);
            //Save NIR
            int NIR = getNIR(fd);
            printf("0x%X saveNIR: %d",address,NIR);
            writeToDatabase("NIR",address,measurment_time,data.NIR);

            close(fd);
        }
    }
}

void getAS7261Mesurements(sensor_list *const s, uint64_t measurment_time){
    
        if (s[i].type == SENSORTYPE_AS7261){
            int fd =  wiringPiI2CSetup(s[i].address);
            if (fd == -1) {
                printf("i2c failed");
            }

            // Save X
            int X = getX(fd);
            printf ( "0x%X getX: %d",s[i].address,X);
            writeToDatabase("X",s[i].address,measurment_time,X);
            // Save Y
            int Y = getY(fd);
            printf ( "0x%X getY: %d",s[i].address,Y);
            writeToDatabase("Y",s[i].address,measurment_time,Y);
            // Save Z
            int Z = getZ(fd);
            printf ( "0x%X getZ: %d",s[i].address,Z);
            writeToDatabase("Z",s[i].address,measurment_time,Z);

            //Save Clear
            int Clear = getClear(fd);
            printf( "0x%X getClear: %d",s[i].address,Clear);
            writeToDatabase("Clear",s[i].address,measurment_time,Clear);
            //Save Dark
            int Dark = getDark(fd);
            printf( "0x%X getDark: %d",s[i].address,Dark);
            writeToDatabase("Dark",s[i].address,measurment_time,Dark);
            //Save NIR
            int NIR = getNIR(fd);
            printf("0x%X getNIR: %d",s[i].address,NIR);
            writeToDatabase("NIR",s[i].address,measurment_time,NIR);

            close(fd);
        }
    
}

void saveAS7265XMesurements(sensor_list *const s, uint64_t measurment_time){
    for(int i = 0 ;s[i].address != -1 && i < 128; i++){ 
        if (s[i].type == SENSORTYPE_AS72651){
            int fd =  wiringPiI2CSetup(s[i].address);
            if (fd == -1) {
                printf("i2c failed");
            }
            // AS72651
            // Save R
            int R = getR(fd);
            printf ( "0x%X getR: %d",s[i].address,R);
            writeToDatabase("R",s[i].address,measurment_time,R);
            // Save S
            int S = getS(fd);
            printf ( "0x%X getS: %d",s[i].address,S);
            writeToDatabase("S",s[i].address,measurment_time,S);
            // Save T
            int T = getT(fd);
            printf ( "0x%X getT: %d",s[i].address,T);
            writeToDatabase("T",s[i].address,measurment_time,T);
            // Save U
            int U = getU(fd);
            printf ( "0x%X getU: %d",s[i].address,U);
            writeToDatabase("U",s[i].address,measurment_time,U);
            // Save V
            int V = getV(fd);
            printf ( "0x%X getV: %d",s[i].address,V);
            writeToDatabase("V",s[i].address,measurment_time,V);
            // Save W
            int W = getW(fd);
            printf ( "0x%X getW: %d",s[i].address,W);
            writeToDatabase("W",s[i].address,measurment_time,W);

            // AS7262
            // Save G
            int G = getG(fd);
            printf ( "0x%X getG: %d",s[i].address,G);
            writeToDatabase("G",s[i].address,measurment_time,G);
            // Save H
            int H = getH(fd);
            printf ( "0x%X getH: %d",s[i].address,H);
            writeToDatabase("H",s[i].address,measurment_time,H);
            // Save I
            int I = getI(fd);
            printf ( "0x%X getI: %d",s[i].address,I);
            writeToDatabase("I",s[i].address,measurment_time,I);
            // Save J
            int J = getJ(fd);
            printf ( "0x%X getJ: %d",s[i].address,J);
            writeToDatabase("J",s[i].address,measurment_time,J);
            // Save K
            int K = getK(fd);
            printf ( "0x%X getK: %d",s[i].address,K);
            writeToDatabase("K",s[i].address,measurment_time,K);
            // Save L
            int L = getL(fd);
            printf ( "0x%X getL: %d",s[i].address,L);
            writeToDatabase("L",s[i].address,measurment_time,L);

            //AS72653
            // Save A
            int A = getA(fd);
            printf ( "0x%X getA: %d",s[i].address,A);
            writeToDatabase("A",s[i].address,measurment_time,A);
            // Save B
            int B = getB(fd);
            printf ( "0x%X getB: %d",s[i].address,B);
            writeToDatabase("B",s[i].address,measurment_time,B);
            // Save C
            int C = getC(fd);
            printf ( "0x%X getC: %d",s[i].address,C);
            writeToDatabase("C",s[i].address,measurment_time,C);
            // Save D
            int D = getD(fd);
            printf ( "0x%X getD: %d",s[i].address,D);
            writeToDatabase("D",s[i].address,measurment_time,D);
            // Save E
            int E = getE(fd);
            printf ( "0x%X getE: %d",s[i].address,E);
            writeToDatabase("E",s[i].address,measurment_time,E);
            // Save F
            int F = getF(fd);
            printf ( "0x%X getF: %d",s[i].address,F);
            writeToDatabase("F",s[i].address,measurment_time,F);
        
            close(fd);
        }
    }
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

void settings(sensor_list *const s, uint8_t integrationValue, uint8_t gain){
    for (int k = 0; s[k].address != -1 && k < 128; ++k){ 
        int fd =  wiringPiI2CSetup(s[k].address);
        if (fd == -1) {
            printf("i2c failed");
        }
        disableInterrupt(fd);
        setIntegrationTime(integrationValue, fd);
        setGain(gain, fd); 
        close(fd);
    }
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
    printf("Set Gain [0:4] 0->1x 1->3.7x 2->16x 3->64x: 4->AutoGain\n");
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

void beginMeasurement(measurmentSettings *Settings, sensor_list *const s,num_AS7261_devices){
    int gain_possibilitys = 4;
    if (Settings->gain != AUTOGAIN){
        gain_possibilitys = 1;
    }
    AS7261_data AS7261_measurments[num_AS7261_devices][gain_possibilitys];
    while(1){
        for (int j = 0; j < gain_possibilitys; ++j){    //Going through every Gain (1/4)
            //apply settings:
            if (Settings->gain == AUTOGAIN){
                //setting integration time from settings Gain is 0-3(j)
                settings(s, Settings->integrationValue, j);
                printf("Gain: %d integrationValue: %d\n", j, Settings->integrationValue );
            }
            else{
                //fixed settings
                settings(s, Settings->integrationValue, Settings->gain);
                printf("Gain: %d integrationValue: %d\n", Settings->gain, Settings->integrationValue );
            }
            
            // Take Measurement
            uint64_t measurment_timestamp = MeasurementFromAllAdresses(s); //Take Measurement and save time stamp
            // read data
            uint64_t read_time = current_timestamp(s); //save time of read start
            for(int i = 0 ;i<num_AS7261_devices; i++){ 
                AS7261_measurments[i] = getAS7261Mesurements(AS7261_measurments[i].adresse)
            }
                //getAS7265XMesurements
        }
        uint64_t read_duration = current_timestamp(s)-read_time; // calculate read_duration
        printf("Read Duration: %lu ms\n",read_duration );
        printf("-----------------------------\n");
        saveAS7261Mesurements(s , measurment_timestamp);  //Save all AS7261
        saveAS7265XMesurements(s , measurment_timestamp); //Save all AS72651 (& AS72652 & AS72653 if present) 
        delayMesuremntMin(Settings->MesuremntIntervall);
    }
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
    int num_AS7261_devices = 0;
    int num_AS72651_devices = 0;
    for(int i = 0 ;s[i].address != -1 && i < 128; i++){ 
        if (s[i].type == SENSORTYPE_AS7261){
            num_AS7261_devices += s[i].num_device_addr;
        }
        else if(s[i].type == SENSORTYPE_AS72651){
            num_AS72651_devices += s[i].num_device_addr; //caluclate the total number of devices
        }
    }
    printf("num_AS7261_devices: %d   num_AS72651_devices:%d\n", num_AS7261_devices);
    beginMeasurement(&Settings,s,num_devices);

    return 0;
}


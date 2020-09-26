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
//#include "../lib/libcsv/src/csv.h"
#include "welcome.h"

#define AUTOGAIN 4
#define GAIN_POSSIBILITIES 3

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

    used_gain.Clear = 0;
    for (int i = 1; i < 4; ++i){
        if (AS7261_measurement[0].Clear < AS7261_measurement[i].Clear && AS7261_measurement[i].Clear < 65500){
            AS7261_measurement[0].Clear = AS7261_measurement[i].Clear;
            used_gain.Clear = i;
        }
    }
    AS7261_measurement[0].Clear = matchValueToMaxGain(used_gain.Clear , AS7261_measurement[0].Clear);

    used_gain.Dark = 0;
    for (int i = 1; i < 4; ++i){
        if (AS7261_measurement[0].Dark < AS7261_measurement[i].Dark && AS7261_measurement[i].Dark < 65500){
            AS7261_measurement[0].Dark = AS7261_measurement[i].Dark;
            used_gain.Dark = i;
        }
    }
    AS7261_measurement[0].Dark = matchValueToMaxGain(used_gain.Dark , AS7261_measurement[0].Dark);

    used_gain.NIR = 0;
    for (int i = 1; i < 4; ++i){
        if (AS7261_measurement[0].NIR < AS7261_measurement[i].NIR && AS7261_measurement[i].NIR < 65500){
            AS7261_measurement[0].NIR = AS7261_measurement[i].NIR;
            used_gain.NIR = i;
        }
    }
    AS7261_measurement[0].NIR = matchValueToMaxGain(used_gain.NIR , AS7261_measurement[0].NIR);
    
    return used_gain;
}

//save the biggest (gain dependet)value that is not clipping
AS7265X_channel cleanAS7265XData(AS7265X_channel *const AS7265X_measurement){
    AS7265X_channel used_gain;
        used_gain.R = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].R < AS7265X_measurement[i].R && AS7265X_measurement[i].R < 65500){
            AS7265X_measurement[0].R = AS7265X_measurement[i].R;
            used_gain.R = i;
        }
    }
    AS7265X_measurement[0].R = matchValueToMaxGain(used_gain.R , AS7265X_measurement[0].R);

    used_gain.S = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].S < AS7265X_measurement[i].S && AS7265X_measurement[i].S < 65500){
            AS7265X_measurement[0].S = AS7265X_measurement[i].S;
            used_gain.S = i;
        }
    }
    AS7265X_measurement[0].S = matchValueToMaxGain(used_gain.S , AS7265X_measurement[0].S);

    used_gain.T = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].T < AS7265X_measurement[i].T && AS7265X_measurement[i].T < 65500){
            AS7265X_measurement[0].T = AS7265X_measurement[i].T;
            used_gain.T = i;
        }
    }
    AS7265X_measurement[0].T = matchValueToMaxGain(used_gain.T , AS7265X_measurement[0].T);

    used_gain.U = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].U < AS7265X_measurement[i].U && AS7265X_measurement[i].U < 65500){
            AS7265X_measurement[0].U = AS7265X_measurement[i].U;
            used_gain.U = i;
        }
    }
    AS7265X_measurement[0].U = matchValueToMaxGain(used_gain.U , AS7265X_measurement[0].U);

    used_gain.V = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].V < AS7265X_measurement[i].V && AS7265X_measurement[i].V < 65500){
            AS7265X_measurement[0].V = AS7265X_measurement[i].V;
            used_gain.V = i;
        }
    }
    AS7265X_measurement[0].V = matchValueToMaxGain(used_gain.V , AS7265X_measurement[0].V);

    used_gain.W = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].W < AS7265X_measurement[i].W && AS7265X_measurement[i].W < 65500){
            AS7265X_measurement[0].W = AS7265X_measurement[i].W;
            used_gain.W = i;
        }
    }
    AS7265X_measurement[0].W = matchValueToMaxGain(used_gain.W , AS7265X_measurement[0].W);

    used_gain.G = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].G < AS7265X_measurement[i].G && AS7265X_measurement[i].G < 65500){
            AS7265X_measurement[0].G = AS7265X_measurement[i].G;
            used_gain.G = i;
        }
    }
    AS7265X_measurement[0].G = matchValueToMaxGain(used_gain.G , AS7265X_measurement[0].G);

    used_gain.H = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].H < AS7265X_measurement[i].H && AS7265X_measurement[i].H < 65500){
            AS7265X_measurement[0].H = AS7265X_measurement[i].H;
            used_gain.H = i;
        }
    }
    AS7265X_measurement[0].H = matchValueToMaxGain(used_gain.H , AS7265X_measurement[0].H);

    used_gain.I = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].I < AS7265X_measurement[i].I && AS7265X_measurement[i].I < 65500){
            AS7265X_measurement[0].I = AS7265X_measurement[i].I;
            used_gain.I = i;
        }
    }
    AS7265X_measurement[0].I = matchValueToMaxGain(used_gain.I , AS7265X_measurement[0].I);

    used_gain.J = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].J < AS7265X_measurement[i].J && AS7265X_measurement[i].J < 65500){
            AS7265X_measurement[0].J = AS7265X_measurement[i].J;
            used_gain.J = i;
        }
    }
    AS7265X_measurement[0].J = matchValueToMaxGain(used_gain.J , AS7265X_measurement[0].J);

    used_gain.K = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].K < AS7265X_measurement[i].K && AS7265X_measurement[i].K < 65500){
            AS7265X_measurement[0].K = AS7265X_measurement[i].K;
            used_gain.K = i;
        }
    }
    AS7265X_measurement[0].K = matchValueToMaxGain(used_gain.K , AS7265X_measurement[0].K);

    used_gain.L = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].L < AS7265X_measurement[i].L && AS7265X_measurement[i].L < 65500){
            AS7265X_measurement[0].L = AS7265X_measurement[i].L;
            used_gain.L = i;
        }
    }
    AS7265X_measurement[0].L = matchValueToMaxGain(used_gain.L , AS7265X_measurement[0].L);

    used_gain.A = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].A < AS7265X_measurement[i].A && AS7265X_measurement[i].A < 65500){
            AS7265X_measurement[0].A = AS7265X_measurement[i].A;
            used_gain.A = i;
        }
    }
    AS7265X_measurement[0].A = matchValueToMaxGain(used_gain.A , AS7265X_measurement[0].A);

    used_gain.B = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].B < AS7265X_measurement[i].B && AS7265X_measurement[i].B < 65500){
            AS7265X_measurement[0].B = AS7265X_measurement[i].B;
            used_gain.B = i;
        }
    }
    AS7265X_measurement[0].B = matchValueToMaxGain(used_gain.B , AS7265X_measurement[0].B);

    used_gain.C = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].C < AS7265X_measurement[i].C && AS7265X_measurement[i].C < 65500){
            AS7265X_measurement[0].C = AS7265X_measurement[i].C;
            used_gain.C = i;
        }
    }
    AS7265X_measurement[0].C = matchValueToMaxGain(used_gain.C , AS7265X_measurement[0].C);

    used_gain.D = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].D < AS7265X_measurement[i].D && AS7265X_measurement[i].D < 65500){
            AS7265X_measurement[0].D = AS7265X_measurement[i].D;
            used_gain.D = i;
        }
    }
    AS7265X_measurement[0].D = matchValueToMaxGain(used_gain.D , AS7265X_measurement[0].D);

    used_gain.E = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].E < AS7265X_measurement[i].E && AS7265X_measurement[i].E < 65500){
            AS7265X_measurement[0].E = AS7265X_measurement[i].E;
            used_gain.E = i;
        }
    }
    AS7265X_measurement[0].E = matchValueToMaxGain(used_gain.E , AS7265X_measurement[0].E);

    used_gain.F = 0;
    for (int i = 1; i <= GAIN_POSSIBILITIES; ++i){
        if (AS7265X_measurement[0].F < AS7265X_measurement[i].F && AS7265X_measurement[i].F < 65500){
            AS7265X_measurement[0].F = AS7265X_measurement[i].F;
            used_gain.F = i;
        }
    }
    AS7265X_measurement[0].F = matchValueToMaxGain(used_gain.F , AS7265X_measurement[0].F);

    return used_gain;
}

AS7261_channel getAS7261Measurement(int address){
    AS7261_channel store_data;
    int fd =  wiringPiI2CSetup(address);
    if (fd == -1) {
        printf("i2c failed");
    }
    // Get X
    store_data.X = getX(fd);
    printf ( "0x%X getX: %d\n",address,store_data.X);
    // Get Y
    store_data.Y = getY(fd);
    printf ( "0x%X getY: %d\n",address,store_data.Y);
    // Get Z
    store_data.Z = getZ(fd);
    printf ( "0x%X getZ: %d\n",address,store_data.Z);
    //Get Clear
    store_data.Clear = getClear(fd);
    printf( "0x%X getClear: %d\n",address,store_data.Clear);

    //Get Dark
    store_data.Dark = getDark(fd);
    printf( "0x%X getDark: %d\n",address,store_data.Dark);

    //Get NIR
    store_data.NIR = getNIR(fd);
    printf("0x%X getNIR: %d\n",address,store_data.NIR);
    

    close(fd);
    return store_data;
}

AS7265X_channel getAS7265XMeasurement(int address){
    AS7265X_channel store_data;
    int fd =  wiringPiI2CSetup(address);
    if (fd == -1) {
        printf("i2c failed");
    }
    // AS7261
    // Save R
    store_data.R = getR(fd);
    printf ( "0x%X getR: %d\n",address,store_data.R);
    // Save S
    store_data.S = getS(fd);
    printf ( "0x%X getS: %d\n",address,store_data.S);
    // Save T
    store_data.T = getT(fd);
    printf ( "0x%X getT: %d\n",address,store_data.T);
    // Save U
    store_data.U = getU(fd);
    printf ( "0x%X getU: %d\n",address,store_data.U);
    // Save V
    store_data.V = getV(fd);
    printf ( "0x%X getV: %d\n",address,store_data.V);
    // Save W
    store_data.W = getW(fd);
    printf ( "0x%X getW: %d\n",address,store_data.W);

    // AS7262
    // Save G
    store_data.G = getG(fd);
    printf ( "0x%X getG: %d\n",address,store_data.G);
    // Save H
    store_data.H = getH(fd);
    printf ( "0x%X getH: %d\n",address,store_data.H);
    // Save I
    store_data.I = getI(fd);
    printf ( "0x%X getI: %d\n",address,store_data.I);
    // Save J
    store_data.J = getJ(fd);
    printf ( "0x%X getJ: %d\n",address,store_data.J);
    // Save K
    store_data.K = getK(fd);
    printf ( "0x%X getK: %d\n",address,store_data.K);
    // Save L
    store_data.L = getL(fd);
    printf ( "0x%X getL: %d\n",address,store_data.L);

    //AS72653
    // Save A
    store_data.A = getA(fd);
    printf ( "0x%X getA: %d\n",address,store_data.A);
    // Save B
    store_data.B = getB(fd);
    printf ( "0x%X getB: %d\n",address,store_data.B);
    // Save C
    store_data.C = getC(fd);
    printf ( "0x%X getC: %d\n",address,store_data.C);
    // Save D
    store_data.D = getD(fd);
    printf ( "0x%X getD: %d\n",address,store_data.D);
    // Save E
    store_data.E = getE(fd);
    printf ( "0x%X getE: %d\n",address,store_data.E);
    // Save F
    store_data.F = getF(fd);
    printf ( "0x%X getF: %d\n",address,store_data.F);

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
    //Save Clear
    printf( "0x%X getClear: %d",address,values.Clear);
    writeToDatabase("Clear",address,measurement_time,values.Clear);
    //Save Dark
    printf( "0x%X getDark: %d",address,values.Dark);
    writeToDatabase("Dark",address,measurement_time,values.Dark);
    //Save NIR
    printf("0x%X getNIR: %d",address,values.NIR);
    writeToDatabase("NIR",address,measurement_time,values.NIR);
}

void saveAS7265XMeasurement(int address,AS7265X_channel values, uint64_t measurement_time){
    // Save R
    printf ( "0x%X saveR: %d",address,values.R);
    writeToDatabase("R",address,measurement_time,values.R);
    // Save S
    printf ( "0x%X saveS: %d",address,values.S);
    writeToDatabase("S",address,measurement_time,values.S);
    // Save T
    printf ( "0x%X saveT: %d",address,values.T);
    writeToDatabase("T",address,measurement_time,values.T);
    // Save U
    printf ( "0x%X saveU: %d",address,values.U);
    writeToDatabase("U",address,measurement_time,values.U);
    // Save V
    printf ( "0x%X saveV: %d",address,values.V);
    writeToDatabase("V",address,measurement_time,values.V);
    // Save W
    printf ( "0x%X saveW: %d",address,values.W);
    writeToDatabase("W",address,measurement_time,values.W);
    // Save G
    printf ( "0x%X saveG: %d",address,values.G);
    writeToDatabase("G",address,measurement_time,values.G);
    // Save H
    printf ( "0x%X saveH: %d",address,values.H);
    writeToDatabase("H",address,measurement_time,values.H);
    // Save I
    printf ( "0x%X saveI: %d",address,values.I);
    writeToDatabase("I",address,measurement_time,values.I);
    // Save J
    printf ( "0x%X saveJ: %d",address,values.J);
    writeToDatabase("J",address,measurement_time,values.J);
    // Save K
    printf ( "0x%X saveK: %d",address,values.K);
    writeToDatabase("K",address,measurement_time,values.K);
    // Save L
    printf ( "0x%X saveL: %d",address,values.L);
    writeToDatabase("L",address,measurement_time,values.L);
    // Save A
    printf ( "0x%X saveA: %d",address,values.A);
    writeToDatabase("A",address,measurement_time,values.A);
    // Save B
    printf ( "0x%X saveB: %d",address,values.B);
    writeToDatabase("B",address,measurement_time,values.B);
    // Save C
    printf ( "0x%X saveC: %d",address,values.C);
    writeToDatabase("C",address,measurement_time,values.C);
    // Save D
    printf ( "0x%X saveD: %d",address,values.D);
    writeToDatabase("D",address,measurement_time,values.D);
    // Save E
    printf ( "0x%X saveE: %d",address,values.E);
    writeToDatabase("E",address,measurement_time,values.E);
    // Save F
    printf ( "0x%X saveF: %d",address,values.F);
    writeToDatabase("F",address,measurement_time,values.F);
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
            printf("X used gain: %d matched value to gain 3: %d\n", used_gain.X, AS7261_measurement[0].X);
            printf("Y used gain: %d matched value to gain 3: %d\n", used_gain.Y, AS7261_measurement[0].Y);
            printf("Z used gain: %d matched value to gain 3: %d\n", used_gain.Z, AS7261_measurement[0].Z);
            printf("Clear used gain: %d matched value to gain 3: %d\n", used_gain.Clear, AS7261_measurement[0].Clear);
            printf("Dark used gain: %d matched value to gain 3: %d\n", used_gain.Dark, AS7261_measurement[0].Dark);
            printf("NIR used gain: %d matched value to gain 3: %d\n", used_gain.NIR, AS7261_measurement[0].NIR);
            saveAS7261Measurement(s[i].address ,AS7261_measurement[0], measurement_time); // write clean values to database
        }
    }
}
void autoGainMeasurementAS7265X(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue){
    AS7265X_channel AS7265X_measurement[4];
    for (int i = 0; s[i].address != -1 && i < 128; ++i){    // going through every device
        if (s[i].type == SENSORTYPE_AS72651){                // only use AS7261 devices
            for (int gain_i = 0; gain_i < 4; ++gain_i){         // going through every gain
                settings(s[i].address, integrationValue, gain_i); // apply settings integrationValue is fix gain is 0-3
                MeasurementFromAdress(s[i].address);            // initiate measurement from current adress
                AS7265X_measurement[gain_i] = getAS7265XMeasurement(s[i].address);    // get data and save to array
            }
            
            AS7265X_channel used_gain = cleanAS7265XData(AS7265X_measurement);   // write clean values to AS7265X_measurement[0] and store used gains
            printf("R used gain: %d matched value to gain 3: %d\n", used_gain.R, AS7265X_measurement[0].R);
            printf("S used gain: %d matched value to gain 3: %d\n", used_gain.S, AS7265X_measurement[0].S);
            printf("T used gain: %d matched value to gain 3: %d\n", used_gain.T, AS7265X_measurement[0].T);
            printf("U used gain: %d matched value to gain 3: %d\n", used_gain.U, AS7265X_measurement[0].U);
            printf("V used gain: %d matched value to gain 3: %d\n", used_gain.V, AS7265X_measurement[0].V);
            printf("W used gain: %d matched value to gain 3: %d\n", used_gain.W, AS7265X_measurement[0].W);
            printf("G used gain: %d matched value to gain 3: %d\n", used_gain.G, AS7265X_measurement[0].G);
            printf("H used gain: %d matched value to gain 3: %d\n", used_gain.H, AS7265X_measurement[0].H);
            printf("I used gain: %d matched value to gain 3: %d\n", used_gain.I, AS7265X_measurement[0].I);
            printf("J used gain: %d matched value to gain 3: %d\n", used_gain.J, AS7265X_measurement[0].J);
            printf("K used gain: %d matched value to gain 3: %d\n", used_gain.K, AS7265X_measurement[0].K);
            printf("L used gain: %d matched value to gain 3: %d\n", used_gain.L, AS7265X_measurement[0].L);
            printf("A used gain: %d matched value to gain 3: %d\n", used_gain.A, AS7265X_measurement[0].A);
            printf("B used gain: %d matched value to gain 3: %d\n", used_gain.B, AS7265X_measurement[0].B);
            printf("C used gain: %d matched value to gain 3: %d\n", used_gain.C, AS7265X_measurement[0].C);
            printf("D used gain: %d matched value to gain 3: %d\n", used_gain.D, AS7265X_measurement[0].D);
            printf("E used gain: %d matched value to gain 3: %d\n", used_gain.E, AS7265X_measurement[0].E);
            printf("F used gain: %d matched value to gain 3: %d\n", used_gain.F, AS7265X_measurement[0].F);

            saveAS7265XMeasurement(s[i].address ,AS7265X_measurement[0], measurement_time); // write clean values to database
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

void manualGainMeasurementAS7265X(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue, uint8_t gain){
    AS7265X_channel AS7265X_measurement;
    for (int i = 0; s[i].address != -1 && i < 128; ++i){    // going through every device
        if (s[i].type == SENSORTYPE_AS72651){                // only use AS7261 devices
            settings(s[i].address, integrationValue, gain); // apply settings integrationValue is fix gain is fix
            MeasurementFromAdress(s[i].address);            // initiate measurement from current adress
            AS7265X_measurement = getAS7265XMeasurement(s[i].address);    // get data and save
            saveAS7265XMeasurement(s[i].address ,AS7265X_measurement, measurement_time); // write values to database
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

    
    close(fd);
}

int main() {

    welcomeMessage();
    printTime();
    char userSettingResponse[4];
    measurmentSettings Settings; //measurmentSettings are stored here 
    //Default Values
    Settings.integrationValue = 128; 
    Settings.gain = 4;
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



    while(1){
        uint64_t measurement_time = current_timestamp(s);       // save mesuremnt time
        if (Settings.gain == AUTOGAIN){
            autoGainMeasurementAS7261(measurement_time, s, Settings.integrationValue);
            autoGainMeasurementAS7265X(measurement_time, s, Settings.integrationValue);
        }
        else{
            manualGainMeasurementAS7261(measurement_time, s, Settings.integrationValue, Settings.gain);
            manualGainMeasurementAS7265X(measurement_time, s, Settings.integrationValue, Settings.gain);
        }
            uint64_t measurement_duration = current_timestamp(s)-measurement_time;
            printf("Measurement duration: %lu ms\n",measurement_duration);
            printf("-----------------------------\n");
            delayMesuremntMin(Settings.MesuremntIntervall);
    }
    return 0;
}


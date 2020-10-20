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


// write settings to device 
// input taget i2c address - integrationValue and gain
void settings(int address, uint8_t integrationValue, uint8_t gain){
    int fd =  wiringPiI2CSetup(address);        // connect to I2C Address and save file disciptor
    if (fd == -1) {                             // check file descriptor for error value
        printf("i2c failed");
    }
    disableInterrupt(fd);                       // disable unuesed interrupt at fd (I2C connection - AS Sensor) 
    setIntegrationTime(integrationValue, fd);   // write integrationValue to fd (I2C connection - AS Sensor) 
    setGain(gain, fd);                          // write gain to fd  (I2C connection - AS Sensor) 
    close(fd);                                  // close connection (I2C connection - AS Sensor) 
    printf("0x%X Changed Gain to %d\n",address,gain );
}


// multiply measurment values to match them to the maximum Gain(x64)
// input value to match and gain used to measure this value
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

AS7261_channel getAS7261Measurement(int address, uint64_t measurement_time, uint8_t gain, uint8_t instant_db_save){
    AS7261_channel store_data;
    int fd =  wiringPiI2CSetup(address);
    if (fd == -1) {
        printf("i2c failed");
    }

    store_data.X = getX(fd);
    store_data.Y = getY(fd);
    store_data.Z = getZ(fd);
    store_data.Clear = getClear(fd);
    store_data.Dark = getDark(fd);
    store_data.NIR = getNIR(fd);

    printf("0x%X getX: %d\n",address,store_data.X);
    printf("0x%X getY: %d\n",address,store_data.Y);
    printf("0x%X getZ: %d\n",address,store_data.Z);
    printf("0x%X getClear: %d\n",address,store_data.Clear);
    printf("0x%X getDark: %d\n",address,store_data.Dark);
    printf("0x%X getNIR: %d\n",address,store_data.NIR);

    if (instant_db_save == 1){
        char buffer[10];
        sprintf(buffer, "X*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.X);
        sprintf(buffer, "Y*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.Y);
        sprintf(buffer, "Z*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.Z);
        sprintf(buffer, "Clear*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.Clear);
        sprintf(buffer, "Dark*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.Dark);
        sprintf(buffer, "NIR*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.NIR);
    }
    
    close(fd);
    return store_data;
}

AS7265X_channel getAS7265XMeasurement(int address, uint64_t measurement_time, uint8_t gain, uint8_t instant_db_save){
    AS7265X_channel store_data;
    int fd =  wiringPiI2CSetup(address);
    if (fd == -1) {
        printf("i2c failed");
    }

    //Get values from Sensor
    store_data.R = getR(fd);
    store_data.S = getS(fd);
    store_data.T = getT(fd);
    store_data.U = getU(fd);
    store_data.V = getV(fd);
    store_data.W = getW(fd);
    store_data.G = getG(fd);
    store_data.H = getH(fd);
    store_data.I = getI(fd);
    store_data.J = getJ(fd);
    store_data.K = getK(fd);
    store_data.L = getL(fd);
    store_data.A = getA(fd);
    store_data.B = getB(fd);
    store_data.C = getC(fd);
    store_data.D = getD(fd);
    store_data.E = getE(fd);
    store_data.F = getF(fd);

    // Print values from Sensor
    printf ( "0x%X getR: %d\n",address,store_data.R);
    printf ( "0x%X getS: %d\n",address,store_data.S);
    printf ( "0x%X getT: %d\n",address,store_data.T);
    printf ( "0x%X getU: %d\n",address,store_data.U);
    printf ( "0x%X getV: %d\n",address,store_data.V);
    printf ( "0x%X getW: %d\n",address,store_data.W);
    printf ( "0x%X getG: %d\n",address,store_data.G);
    printf ( "0x%X getH: %d\n",address,store_data.H);
    printf ( "0x%X getI: %d\n",address,store_data.I);
    printf ( "0x%X getJ: %d\n",address,store_data.J);
    printf ( "0x%X getK: %d\n",address,store_data.K);
    printf ( "0x%X getL: %d\n",address,store_data.L);
    printf ( "0x%X getA: %d\n",address,store_data.A);
    printf ( "0x%X getB: %d\n",address,store_data.B);
    printf ( "0x%X getC: %d\n",address,store_data.C);
    printf ( "0x%X getD: %d\n",address,store_data.D);
    printf ( "0x%X getE: %d\n",address,store_data.E);
    printf ( "0x%X getF: %d\n",address,store_data.F);

    // instantly save values to Database
    if (instant_db_save == 1){
        char buffer[5];
        sprintf(buffer, "R*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.R);
        sprintf(buffer, "S*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.S);
        sprintf(buffer, "T*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.T);
        sprintf(buffer, "U*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.U);
        sprintf(buffer, "V*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.V);
        sprintf(buffer, "W*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.W);
        sprintf(buffer, "G*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.G);
        sprintf(buffer, "H*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.H);
        sprintf(buffer, "I*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.I);
        sprintf(buffer, "J*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.J);
        sprintf(buffer, "K*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.K);
        sprintf(buffer, "L*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.L);
        sprintf(buffer, "A*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.A);
        sprintf(buffer, "B*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.B);
        sprintf(buffer, "C*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.C);
        sprintf(buffer, "D*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.D);
        sprintf(buffer, "E*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.E);
        sprintf(buffer, "F*%d", gain);
        writeToDatabase(buffer,address,measurement_time,store_data.F);
    }

    close(fd);
    return store_data;
}

void saveAS7261Measurement(int address,AS7261_channel values, uint64_t measurement_time){
    writeToDatabase("X",address,measurement_time,values.X);
    writeToDatabase("Y",address,measurement_time,values.Y);
    writeToDatabase("Z",address,measurement_time,values.Z);
    writeToDatabase("Clear",address,measurement_time,values.Clear);
    writeToDatabase("Dark",address,measurement_time,values.Dark);
    writeToDatabase("NIR",address,measurement_time,values.NIR);

    printf("0x%X saveX: %d\n",address,values.X);
    printf("0x%X saveY: %d\n",address,values.Y);
    printf("0x%X saveZ: %d\n",address,values.Z);
    printf("0x%X getClear: %d\n",address,values.Clear);
    printf("0x%X getDark: %d\n",address,values.Dark);
    printf("0x%X getNIR: %d\n",address,values.NIR);
}

void saveAS7265XMeasurement(int address, AS7265X_channel values, uint64_t measurement_time){
    writeToDatabase("R",address,measurement_time,values.R);
    writeToDatabase("S",address,measurement_time,values.S);
    writeToDatabase("T",address,measurement_time,values.T);
    writeToDatabase("U",address,measurement_time,values.U);
    writeToDatabase("V",address,measurement_time,values.V);
    writeToDatabase("W",address,measurement_time,values.W);
    writeToDatabase("G",address,measurement_time,values.G);
    writeToDatabase("H",address,measurement_time,values.H);
    writeToDatabase("I",address,measurement_time,values.I);
    writeToDatabase("J",address,measurement_time,values.J);
    writeToDatabase("K",address,measurement_time,values.K);
    writeToDatabase("L",address,measurement_time,values.L);
    writeToDatabase("A",address,measurement_time,values.A);
    writeToDatabase("B",address,measurement_time,values.B);
    writeToDatabase("C",address,measurement_time,values.C);
    writeToDatabase("D",address,measurement_time,values.D);
    writeToDatabase("E",address,measurement_time,values.E);
    writeToDatabase("F",address,measurement_time,values.F);

    printf ( "0x%X saveR: %d\n",address,values.R);
    printf ( "0x%X saveS: %d\n",address,values.S);
    printf ( "0x%X saveT: %d\n",address,values.T);
    printf ( "0x%X saveU: %d\n",address,values.U);
    printf ( "0x%X saveV: %d\n",address,values.V);
    printf ( "0x%X saveW: %d\n",address,values.W);
    printf ( "0x%X saveG: %d\n",address,values.G);
    printf ( "0x%X saveH: %d\n",address,values.H);
    printf ( "0x%X saveI: %d\n",address,values.I);
    printf ( "0x%X saveJ: %d\n",address,values.J);
    printf ( "0x%X saveK: %d\n",address,values.K);
    printf ( "0x%X saveL: %d\n",address,values.L);
    printf ( "0x%X saveA: %d\n",address,values.A);
    printf ( "0x%X saveB: %d\n",address,values.B);
    printf ( "0x%X saveC: %d\n",address,values.C);
    printf ( "0x%X saveD: %d\n",address,values.D);
    printf ( "0x%X saveE: %d\n",address,values.E);
    printf ( "0x%X saveF: %d\n",address,values.F);
}

void saveAS7261Gain(int address,AS7261_channel gain, uint64_t measurement_time){
    writeToDatabase("gainX",address,measurement_time,gain.X);
    writeToDatabase("gainY",address,measurement_time,gain.Y);
    writeToDatabase("gainZ",address,measurement_time,gain.Z);
    writeToDatabase("gainClear",address,measurement_time,gain.Clear);
    writeToDatabase("gainDark",address,measurement_time,gain.Dark);
    writeToDatabase("gainNIR",address,measurement_time,gain.NIR);
}

void autoGainMeasurementAS7261(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue){

    AS7261_channel AS7261_measurement[4];
    for (int i = 0; s[i].address != -1 && i < 128; ++i){    // going through every device
        if (s[i].type == SENSORTYPE_AS7261){                // only use AS7261 devices
            for (uint8_t gain_i = 0; gain_i < 4; ++gain_i){         // going through every gain
                settings(s[i].address, integrationValue, gain_i); // apply settings integrationValue is fix gain is 0-3
                MeasurementFromAdress(s[i].address);            // initiate measurement from current adress
                AS7261_measurement[gain_i] = getAS7261Measurement(s[i].address, measurement_time, gain_i, 1);    // get data and save to array
            }
            
            AS7261_channel used_gain = cleanAS7261Data(AS7261_measurement);   // write clean values to AS7261_measurement[0] and store used gains
            printf("X used gain: %d matched value to gain 3: %d\n", used_gain.X, AS7261_measurement[0].X);
            printf("Y used gain: %d matched value to gain 3: %d\n", used_gain.Y, AS7261_measurement[0].Y);
            printf("Z used gain: %d matched value to gain 3: %d\n", used_gain.Z, AS7261_measurement[0].Z);
            printf("Clear used gain: %d matched value to gain 3: %d\n", used_gain.Clear, AS7261_measurement[0].Clear);
            printf("Dark used gain: %d matched value to gain 3: %d\n", used_gain.Dark, AS7261_measurement[0].Dark);
            printf("NIR used gain: %d matched value to gain 3: %d\n", used_gain.NIR, AS7261_measurement[0].NIR);
            saveAS7261Measurement(s[i].address ,AS7261_measurement[0], measurement_time); // write clean values to database
            saveAS7261Gain(s[i].address, used_gain, measurement_time);
        }
    }
}

void saveAS7265XGain(int address,AS7265X_channel gain, uint64_t measurement_time){
    writeToDatabase("gainR",address,measurement_time,gain.R);
    writeToDatabase("gainS",address,measurement_time,gain.S);
    writeToDatabase("gainT",address,measurement_time,gain.T);
    writeToDatabase("gainU",address,measurement_time,gain.U);
    writeToDatabase("gainV",address,measurement_time,gain.V);
    writeToDatabase("gainW",address,measurement_time,gain.W);
    writeToDatabase("gainG",address,measurement_time,gain.G);
    writeToDatabase("gainH",address,measurement_time,gain.H);
    writeToDatabase("gainI",address,measurement_time,gain.I);
    writeToDatabase("gainJ",address,measurement_time,gain.J);
    writeToDatabase("gainK",address,measurement_time,gain.K);
    writeToDatabase("gainL",address,measurement_time,gain.L);
    writeToDatabase("gainA",address,measurement_time,gain.A);
    writeToDatabase("gainB",address,measurement_time,gain.B);
    writeToDatabase("gainC",address,measurement_time,gain.C);
    writeToDatabase("gainD",address,measurement_time,gain.D);
    writeToDatabase("gainE",address,measurement_time,gain.E);
    writeToDatabase("gainF",address,measurement_time,gain.F);

}
void autoGainMeasurementAS7265X(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue){
    AS7265X_channel AS7265X_measurement[4];
    for (int i = 0; s[i].address != -1 && i < 128; ++i){    // going through every device
        if (s[i].type == SENSORTYPE_AS72651){                // only use AS7261 devices
            for (int gain_i = 0; gain_i < 4; ++gain_i){         // going through every gain
                settings(s[i].address, integrationValue, gain_i); // apply settings integrationValue is fix gain is 0-3
                MeasurementFromAdress(s[i].address);            // initiate measurement from current adress
                AS7265X_measurement[gain_i] = getAS7265XMeasurement(s[i].address, measurement_time, gain_i, 1);    // get data and save to array
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
            saveAS7265XGain(s[i].address, used_gain, measurement_time);
        }
    }
}

void manualGainMeasurementAS7261(uint64_t measurement_time, sensor_list *const s, uint8_t integrationValue, uint8_t gain){
    AS7261_channel AS7261_measurement;
    for (int i = 0; s[i].address != -1 && i < 128; ++i){    // going through every device
        if (s[i].type == SENSORTYPE_AS7261){                // only use AS7261 devices
            settings(s[i].address, integrationValue, gain); // apply settings
            MeasurementFromAdress(s[i].address);            // initiate measurement from current adress
            AS7261_measurement = getAS7261Measurement(s[i].address, measurement_time, gain, 0);    // get data and save
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
            AS7265X_measurement = getAS7265XMeasurement(s[i].address, measurement_time, gain, 0);    // get data and save
            saveAS7265XMeasurement(s[i].address ,AS7265X_measurement, measurement_time); // write values to database
        }
    }
}


cmake_minimum_required(VERSION 3.9)
project(Code)

set(CMAKE_CXX_STANDARD 11)

add_executable(Code
        SparkFun_AS726X_Arduino_Library-master/examples/Example1_BasicReadings/Example1_BasicReadings.ino
        SparkFun_AS726X_Arduino_Library-master/examples/Example2_SensorSettings/Example2_SensorSettings.ino
        SparkFun_AS726X_Arduino_Library-master/examples/Example3_BulbReadings/Example3_BulbReadings.ino
        SparkFun_AS726X_Arduino_Library-master/extras/14351-01.jpg
        SparkFun_AS726X_Arduino_Library-master/src/AS726X.cpp
        SparkFun_AS726X_Arduino_Library-master/src/AS726X.h AS7261-test/AS726X.c wiringPi_AS726X_Libary/AS726X.h wiringPi_AS726X_Libary/main.c wiringPi_AS726X_Libary/influxdb.c)

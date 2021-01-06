# Spectral Measurement Platform
## This is my Bachelorthesis: "Entwicklung und Realisierung eines Messsystems mit den Sensoren AS7261 und AS72651 von AMS"
Work in Progress


<img align="left" src="https://github.com/LennardBoediger/Bachelorarbeit/blob/master/Latex/Bachelorarbeit/img/Fancy-Product.JPG" width="300"/>
The goal of this work is to develop a spectral sensor platform using the AS7261 and AS72651, which can be used modularly for different 
research purposes, but especially for sky measurements. Since it is not possible to measure in several directions in the sky at the same 
time with only one of the AS7261 (or AS72651) sensors without mechanical components, it must be ensured that a large number of sensors can be 
combined into a sensor array in order to be able to measure with directional resolution. The resulting sensor platform uses a Raspberry Pi 4 as central 
control unit, which communicates with external sensor boards via I2C.

The final sensor platform will be able to differentiate 21 spectral channels of incident light from up to 20 different directions.


## Grafana WebInterface 

<img src="https://github.com/LennardBoediger/Bachelorarbeit/blob/master/Latex/Bachelorarbeit/img/Grafana-Product.jpg" alt="left" width="700"/>

## Auto Gain
To increase the usable light intensity range, the AutoGain mode can be used.</br>
Measurements are made in all 4 possible gains, the most accurate measured value is selected for each channel. This value is adjusted to the maximum gain with the function matchValueTo-MaxGain to allow a continuous representation of the output.
```c
for (uint8_t gain_i = 0; gain_i < 4; ++gain_i){         // going through every gain
                settings(s[i].address, integrationValue, gain_i); // apply settings integrationValue is fix gain is 0-3
                MeasurementFromAdress(s[i].address);            // initiate measurement from current adress
                AS7261_measurement[gain_i] = getAS7261Measurement(s[i].address, measurement_time, gain_i, 1);    // get data and save to array
            }
            
```
## Usage
Flash the SD image</br>
Attach 1-20 Sensorboards in sereries</br>
Connect to the Pi over SSH</br>
Attatch the screen with `screen -r`</br>
Follow the instructions to change the setting and start the measurement</br>
The Webinterface can be found at port 300 `http://[RASPBERRY_PI-IP]:3000`
<img src="https://github.com/LennardBoediger/Bachelorarbeit/blob/master/Latex/Bachelorarbeit/img/handbuch/check_settings.png" alt="left" width="500"/>

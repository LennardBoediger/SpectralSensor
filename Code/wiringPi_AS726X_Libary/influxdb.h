#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
/*Influxdb Config*/
#define PORT 8086							/* Port number as an integer - web server default is 80 */
#define IP_ADDRESS "127.0.0.1"				/* IP Address as a string */
#define DATABASE "SpectralSensor"			/* This is the InfluxDB database name */
#define USERNAME "Input"					/* These are the credentials used to access the database */
#define PASSWORD "299792458"
#define HOSTNAME "NanoPi-NEO2-Black"
#define BUFSIZE 8196

int writeToDatabase(char measurement[],int i2c_adrress,int value);
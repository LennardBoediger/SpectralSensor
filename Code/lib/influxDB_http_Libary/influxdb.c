#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>


/* InfuxDB Config */
#define IP_ADDRESS "127.0.0.1"          // IP Address as a string
#define PORT        8086                // Port number as an integer
#define DATABASE "SpectralSensor"       // InfluxDB database name
#define USERNAME "Input"                // Login Data Username
#define PASSWORD "299792458"            // Login Data password

/* client endpoint details for a tag: replace with your hostname or use gethostname() */
#define HOSTNAME "NanoPi-NEO2-Black"
#define LOOPS   100
#define BUFSIZE 8196
#define DEBUG   0

#define SUCSESS 1
#define FAIL 0

int pexit(char * msg)
{
    perror(msg);
    exit(1);
}

int writeToDatabase(char measurement[],int i2c_adrress, uint64_t measurment_time_ms ,int value)
{
    int sockfd;
    int ret;
    char header[BUFSIZE];
    char body[BUFSIZE];
    char result[BUFSIZE];

    static struct sockaddr_in serv_addr; /* static is zero filled on start up */
    if (DEBUG == 1){
        printf("Connecting socket to %s and port %d\n", IP_ADDRESS, PORT);
    }
    if((sockfd = socket(AF_INET, SOCK_STREAM,0)) <0)
        pexit("socket() failed");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    serv_addr.sin_port = htons(PORT);
    
    //  Create InfluxDB line protocol body
    //  1. measurement Name
    //  2. tag = device identifier
    //  3. value 
    //  4. epoch time missing (3 spaces) so InfluxDB generates the timestamp
    if (DEBUG == 1){
        printf("%s,i2c_adrress=%x value=%i %llu\n", measurement,i2c_adrress ,value, (unsigned long long)measurment_time_ms);
    }
    sprintf(body, "%s,i2c_adrress=%x value=%i %llu\n", measurement,i2c_adrress ,value, (unsigned long long)measurment_time_ms);

    // Create InfluxDB line protocol header
    // Note spaces taht spaces, carriage-returns & newlines are important
    // db= is the datbase name, precision=timestamp precision , u= the username and p= the password
    sprintf(header,"POST /write?db=%s&precision=ms&u=%s&p=%s HTTP/1.1\r\nHost: influx:8086\r\nContent-Length: %d\r\n\r\n", 
        DATABASE, USERNAME, PASSWORD, strlen(body));

    uint8_t done = FAIL; 
    uint8_t fail_count = 0;
    while(done != SUCSESS){

        if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <0)
            pexit("connect() failed");

        //printf("Send to InfluxDB the POST request bytes=%d \n->|%s|<-\n",strlen(header), header);
        ret = write(sockfd, header, strlen(header));
        if (ret < 0)
            pexit("Write Header request to InfluxDB failed");

        if(DEBUG == 1){
            printf("Send to InfluxDB the data bytes=%d \n->|%s|<-\n",strlen(body), body);
        }
        ret = write(sockfd, body, strlen(body));
        if (ret < 0)
            pexit("Write Data Body to InfluxDB failed");

        // Get back the acknwledgement from InfluxDB //
        // "HTTP/1.1 204 No Content" is the expectet server return string
        ret = read(sockfd, result, sizeof(result));
        if (ret < 0){
            pexit("Reading the result from InfluxDB failed");
        }
        result[ret] = 0; // terminate string
        if(DEBUG == 1){
            printf("Result returned:%s\n",result);
        }else{
            if (result[9] == '2' && result[10] == '0' && result[11] == '4'){
                done = SUCSESS;
               // printf("  OK\n");
            }
            else{
                printf("Error: %c%c%c\n",result[9],result[10],result[11]);
                fail_count++;

            }
        }

        if(close(sockfd) < 0){
            pexit("close failed");
        }
        sleep(5*fail_count);
        if (fail_count > 6)
        {
            pexit("Write to Database Failed!");
        }
    }
    return 0;
}
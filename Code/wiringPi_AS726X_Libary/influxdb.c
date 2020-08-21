/* This is sample C code as an example */
/* Example of loading stats data into InfluxDB in its Line Protocol format over a network using HTTP POST */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

/* YOU WILL HAVE TO CHANGE THESE FIVE LINES TO MATCH YOUR INFLUXDB CONFIG */
#define PORT        8086                   /* Port number as an integer - web server default is 80 */
#define IP_ADDRESS "127.0.0.1"  /* IP Address as a string */
#define DATABASE "SpectralSensor"                /* This is the InfluxDB database name */
#define USERNAME "Input"               /* These are the credentials used to access the database */
#define PASSWORD "299792458"

/* client endpoint details for a tag: replace with your hostname or use gethostname() */
#define HOSTNAME "NanoPi-NEO2-Black"
#define SECONDS 1
#define LOOPS   100
#define BUFSIZE 8196

int pexit(char * msg)
{
    perror(msg);
    exit(1);
}

int writeToDatabase(char measurement, int value)
{
    int sockfd;
    int ret;
    char header[BUFSIZE];
    char body[BUFSIZE];
    char result[BUFSIZE];

    static struct sockaddr_in serv_addr; /* static is zero filled on start up */

    printf("Connecting socket to %s and port %d\n", IP_ADDRESS, PORT);
    if((sockfd = socket(AF_INET, SOCK_STREAM,0)) <0)
        pexit("socket() failed");

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(IP_ADDRESS);
    serv_addr.sin_port = htons(PORT);
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <0)
        pexit("connect() failed");

    /* InfluxDB line protocol note:
        measurement name is "noise"
        tag is host=blue - multiple tags separate with comma
        data is random=<number>
        ending epoch time missing (3 spaces) so InfluxDB generates the timestamp */
    /* InfluxDB line protocol note: ending epoch time missing so InfluxDB greates it */
    sprintf(body, "noise,measurement=%s value=%i   \n", HOSTNAME, value);

    /* Note spaces are important and the carriage-returns & newlines */
    /* db= is the datbase name, u= the username and p= the password */
    sprintf(header,"POST /write?db=%s&u=%s&p=%s HTTP/1.1\r\nHost: influx:8086\r\nContent-Length: %ld\r\n\r\n", 
        DATABASE, USERNAME, PASSWORD, strlen(body));

    printf("Send to InfluxDB the POST request bytes=%d \n->|%s|<-\n",strlen(header), header);
    ret = write(sockfd, header, strlen(header));
    if (ret < 0)
        pexit("Write Header request to InfluxDB failed");

    printf("Send to InfluxDB the data bytes=%d \n->|%s|<-\n",strlen(body), body);
    ret = write(sockfd, body, strlen(body));
    if (ret < 0)
        pexit("Write Data Body to InfluxDB failed");

    /* Get back the acknwledgement from InfluxDB */
    /* It worked if you get "HTTP/1.1 204 No Content" and some other fluff */
    ret = read(sockfd, result, sizeof(result));
    if (ret < 0)
        pexit("Reading the result from InfluxDB failed");
    result[ret] = 0; /* terminate string */
    printf("Result returned from InfluxDB. Note:204 is Sucess\n->|%s|<-\n",result);

    close(sockfd);
    return 0;
}
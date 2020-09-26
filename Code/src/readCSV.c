#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// adjust BUFFER_SIZE to suit longest line 
#define BUFFER_SIZE 1024 * 1024
#define NUM_FIELDS 10
#define MAXERRS 5
#define RET_OK 0
#define RET_FAIL 1
#define FALSE 0
#define TRUE 1

// char* array will point to fields
char *pFields[NUM_FIELDS];
// field offsets into pFields array:
#define LP          0
#define IMIE        1
#define NAZWISKo    2
#define ULICA       3
#define NUMER       4
#define KOD         5
#define MIEJSCOw    6
#define TELEFON     7
#define EMAIL       8
#define DATA_UR     9

long loadFile(FILE *pFile, long *errcount);
static int  loadValues(char *line, long lineno);
static char delim;

long loadFile(FILE *pFile, long *errcount){

    char sInputBuf [BUFFER_SIZE];
    long lineno = 0L;

    if(pFile == NULL)
        return RET_FAIL;

    while (!feof(pFile)) {

        // load line into static buffer
        if(fgets(sInputBuf, BUFFER_SIZE-1, pFile)==NULL)
            break;

        // skip first line (headers)
        if(++lineno==1)
            continue;

        // jump over empty lines
        if(strlen(sInputBuf)==0)
            continue;
        // set pFields array pointers to null-terminated string fields in sInputBuf
        if(loadValues(sInputBuf,lineno)==RET_FAIL){
           (*errcount)++;
            if(*errcount > MAXERRS)
                break;
        } else {    
            // On return pFields array pointers point to loaded fields ready for load into DB or whatever
            // Fields can be accessed via pFields, e.g.
            printf("lp=%s, imie=%s, data_ur=%s\n", pFields[LP], pFields[IMIE], pFields[DATA_UR]);
        }
    }
    return lineno;
}


static int  loadValues(char *line, long lineno){
    if(line == NULL)
        return RET_FAIL;

    // chop of last char of input if it is a CR or LF (e.g.Windows file loading in Unix env.)
    // can be removed if sure fgets has removed both CR and LF from end of line
    if(*(line + strlen(line)-1) == '\r' || *(line + strlen(line)-1) == '\n')
        *(line + strlen(line)-1) = '\0';
    if(*(line + strlen(line)-1) == '\r' || *(line + strlen(line)-1 )== '\n')
        *(line + strlen(line)-1) = '\0';

    char *cptr = line;
    int fld = 0;
    int inquote = FALSE;
    char ch;

    pFields[fld]=cptr;
    while((ch=*cptr) != '\0' && fld < NUM_FIELDS){
        if(ch == '"') {
            if(! inquote)
                pFields[fld]=cptr+1;
            else {
                *cptr = '\0';               // zero out " and jump over it
            }
            inquote = ! inquote;
        } else if(ch == delim && ! inquote){
            *cptr = '\0';                   // end of field, null terminate it
            pFields[++fld]=cptr+1;
        }
        cptr++;
    }   
    if(fld > NUM_FIELDS-1){
        fprintf(stderr, "Expected field count (%d) exceeded on line %ld\n", NUM_FIELDS, lineno);
        return RET_FAIL;
    } else if (fld < NUM_FIELDS-1){
        fprintf(stderr, "Expected field count (%d) not reached on line %ld\n", NUM_FIELDS, lineno);
        return RET_FAIL;    
    }
    return RET_OK;
}

int main(int argc, char **argv)
{
   FILE *fp;
   long errcount = 0L;
   long lines = 0L;

   if(argc!=3){
       printf("Usage: %s csvfilepath delimiter\n", basename(argv[0]));
       return (RET_FAIL);
   }   
   if((delim=argv[2][0])=='\0'){
       fprintf(stderr,"delimiter must be specified\n");
       return (RET_FAIL);
   }
   fp = fopen(argv[1] , "r");
   if(fp == NULL) {
      fprintf(stderr,"Error opening file: %d\n",errno);
      return(RET_FAIL);
   }
   lines=loadFile(fp,&errcount);
   fclose(fp);
   printf("Processed %ld lines, encountered %ld error(s)\n", lines, errcount);
   if(errcount>0)
        return(RET_FAIL);
    return(RET_OK); 
}
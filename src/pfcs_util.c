/* -------------------------------------------------------------------------- */
/* pfcs_util.h                                                                */
/* -------------------------------------------------------------------------- */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <uuid/uuid.h>
#include "log.h"
#include "gpgll.h"
#include "pfcs_util.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                     */
/* -------------------------------------------------------------------------- */
#define PATH_PID_PFCS "./pid_pfcs.tmp"
#define PATH_LAST_READING "./tmp/reading.tmp"
#define MAX_ROW_LEN 100 /* Max length of row of file G18.txt */
#define UUID_LEN 37 /* Length of UUID */
#define READING_ROW_LEN UUID_LEN + MAX_ROW_LEN + 2
#define LOG_READER "PFC_READER"
#define MSG_CREATEPFCS_START_PROC "createPFCS - Start  proc: "
#define MSG_CREATEPFCS_FINISH_PROC "createPFCS - Finish proc: "
#define MSG_WRITECOORD_OPEN_FILE_TMP "writeCoord - Open  file: temporary file"
#define MSG_WRITECOORD_CLOSE_FILE_TMP "writeCoord - Close file: temporary file"
#define MSG_WRITECOORD_WRITE_COORD "writeCoord - Write coord: "
#define MSG_WRITECOORD_ERR_OPEN_FILE_TMP "writeCoord - Error opening temporary file"
#define MSG_WRITECOORD_ERR_WRITE_FILE_TMP "writeCoord - Error writing temporary file"

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

/*
 * DESCRIPTION
 * This function builds the specific message for the start of a process.
 *
 * PARAMETERS:
 * - msg = Message
 * - text = Text to include inside the messege
 *
 * RETURN VALUES
 * Upon successful completion, buildCreatePFCSStartMsg returns a pointer to the
 * string that contains the message.
 */
char* buildCreatePFCSStartMsg(char *msg, char *text)
{
  strcpy(msg, MSG_CREATEPFCS_START_PROC);
  strcat(msg, text);
  return msg;
}

/*
 * DESCRIPTION
 * This function builds the specific message for the finish of a process.
 *
 * PARAMETERS:
 * - msg = Message
 * - text = Text to include inside the messege
 *
 * RETURN VALUES
 * Upon successful completion, buildCreatePFCSFinishMsg returns a pointer to the
 * string that contains the message.
 */
char* buildCreatePFCSFinishMsg(char *msg, char *text)
{
  strcpy(msg, MSG_CREATEPFCS_FINISH_PROC);
  strcat(msg, text);
  return msg;
}

/*
 * DESCRIPTION
 * This function builds the message for the temporary file that contains the
 * last coordinate read.
 *
 * PARAMETERS:
 * - msg = ...
 * - text = ...
 *
 * RETURN VALUES:
 * Upon successful completion, buildReadingMsg returns a pointer to the
 * string that contains the message.
 */
char* buildReadingMsg(char *msg, char *text)
{
  uuid_t binuuid;
  char uuid[UUID_LEN];
  uuid_generate_random(binuuid);
  uuid_unparse_upper(binuuid, uuid);
  strcpy(msg, uuid);
  strcat(msg, "::");
  strcat(msg, text);
  return msg;
}

/*
 * DESCRIPTION
 * This function write the content of the temporary file 'reanding.tmp'.
 *
 * PARAMETERS:
 * - reading = Coordinate read from the GPGLL file.
 *
 * RETURN VALUES:
 * - 0 = All right;
 * - 1 = Error opening temporary file
 */
int writeCoord(char *reading)
{
  FILE *fpTmp;
  char logMsg[strlen(MSG_WRITECOORD_WRITE_COORD) + strlen(reading)];

  fpTmp = fopen(PATH_LAST_READING, "w"); /* Open temporary file in write mode */
  if (fpTmp == NULL) {
    addLog(LOG_READER, MSG_WRITECOORD_ERR_OPEN_FILE_TMP);
     return 1;
  }
  addLog(LOG_READER, MSG_WRITECOORD_OPEN_FILE_TMP);
  fputs(reading, fpTmp);
  fclose(fpTmp); /* Close temporary file */
  strcpy(logMsg, MSG_WRITECOORD_WRITE_COORD);
  strcat(logMsg, reading);
  addLog(LOG_READER, logMsg);
  addLog(LOG_READER, MSG_WRITECOORD_CLOSE_FILE_TMP);
  return 0;
}

/*
 * DESCRIPTION
 * This function read the content of the temporary file 'reanding.tmp'.
 *
 * PARAMETERS:
 * - coord
 *
 * RETURN VALUES:
 * Upon successful completion, readCoord returns a pointer to the string that
 * contains the UUID plus GPGLL. Otherwise, return NULL.
 */
char* readCoord(char *coord)
{
  FILE *fpTmp;

  fpTmp = fopen(PATH_LAST_READING, "r"); /* Open temporary file read mode */
  if (fpTmp == NULL) {
    return NULL;
  }
  if (fgets(coord, READING_ROW_LEN, fpTmp) == NULL) {
    return NULL;
  }
  fclose(fpTmp); /* Close temporary file */
  return coord;
}

/*
 * DESCRIPTION
 * This function reads UUID string from Coordinate string.
 *
 * PARAMETERS:
 * - uuid = UUID
 * - coord = Coordinate
 *
 * RETURN VALUES:
 * Upon successful completion, getUUID returns a pointer to the string that
 * contains the UUID. Otherwise, return NULL.
 */
char* getUUID(char *uuid, char *coord)
{
  if (getSubStr(coord, uuid, 0, 35) == 0) {
    return uuid;
  } else {
    return NULL;
  }
}

/*
 * DESCRIPTION
 * This function reads GPGLL string from Coordinate string.
 *
 * PARAMETERS:
 * - gpgll = GPGLL
 * - coord = Coordinate
 *
 * RETURN VALUES:
 * Upon successful completion, getGPGLL returns a pointer to the string that
 * contains the GPGLL. Otherwise, return NULL.
 */
char* getGPGLL(char *gpgll, char *coord)
{
  if (getSubStr(coord, gpgll, 39, 82) == 0) {
    return gpgll;               /* GPGLL */
  } else if (getSubStr(coord, gpgll, 38, 42) == 0) {
    return gpgll;               /* STRAT */
  } else if (getSubStr(coord, gpgll, 38, 40) == 0) {
    return gpgll;               /* END */
  } else {
    return NULL;
  }
}

/*
 * DESCRIPTION
 * This function return latitude from GPGLL string.
 *
 * PARAMETERS:
 * - gpgll = GPGLL string
 *
 * RETURN VALUES:
 * returns a double with the latitude contains inside GPGLL string.
 */
double getLat(char *gpgll)
{
  char sLat[10];
  double dLat = 0;
  getSubStr(gpgll, sLat, 7, 15);
  dLat = atof(sLat) / 10;
  return dLat;
}

/*
 * DESCRIPTION
 * This function return longitude from GPGLL string.
 *
 * PARAMETERS:
 * - gpgll = GPGLL string
 *
 * RETURN VALUES:
 * returns a double with the longitude contains inside GPGLL string.
 */
double getLon(char *gpgll)
{
  char sLon[11];
  double dLon = 0;
  if (strlen(gpgll) )
  getSubStr(gpgll, sLon, 19, 28);
  dLon = atof(sLon) / 10;
  return dLon;
}

/*
 * DESCRIPTION
 * This function gets a substring from a suorce string.
 *
 * PARAMETERS:
 * - source =  Source string which will be read from the user and provided to
 *             the function as an argument;
 * - target = Target string, substring will be assigned in this variable;
 * - from = Start index from where we will extract the substring (it should be
 *          greater than or equal to 0 and less than string length);
 * - to = End index, the last character of the substring (it should be less than
 *        string length)
 *
 * RETURN VALUES:
 * - 0 = everything's ok
 * - 1 = Invalid 'from' index, this condition happens when from is minor
 *       than 0 or it's greater than the length of the source string;
 * - 2 = Invalid 'to' index, this condition happens when to id greater
 *       than the length of the source string;
 * - 3 = Invalid 'from' and 'to' index, this condition happens when the
 *       from index greater than to index.
 */
int getSubStr(char *source, char *target,int from, int to)
{
  int length=0;
  int i=0,j=0;

  /* get length of source string with out '\0' */
  while(source[i++]!='\0')
    length++;

  if(from<0 || from>length) {
    return 1; /* Invalid 'from' index" */
  }

  if(to>length) {
    return 2; /* Invalid 'to' index */
  }

  if (from>to) {
    return 3; /* Invalid 'from' and 'to' index */
  }

  for(i=from, j=0; i<=to; i++, j++) {
    target[j]=source[i];
  }

  //assign NULL at the end of string
  target[j]='\0';

  return 0;
}

/*
 * DESCRIPTION
 * Sleep for the requested number of milliseconds.
 *
 * PARAMETERS:
 * - msec = Numbers of milliseconds
 *
 * RETURN VALUES:
 * Upon successful completion, mssleep returns 0. Otherwise, return a value
 * different than 0.
 */
int mssleep(long msec)
{
  struct timespec ts;
  int res;

  if (msec < 0)
    {
      return -1;
    }

  ts.tv_sec = msec / 1000;      /* Numbers of seconds */
  ts.tv_nsec = (msec % 1000) * 1000000; /* Numbers of nanoseconds */

  do {
    res = nanosleep(&ts, &ts);
  } while (res && errno == EINTR);

  return res;
}

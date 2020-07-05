/* -------------------------------------------------------------------------- */
/* pfcs_util.c                                                                */
/* -------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "log.h"
#include "gpgll.h"
#include "pfcs_util.h"
#include "string_util.h"
#include "velocity_util.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define PATH_PID_PFCS "./pid_pfcs.tmp"
#define PATH_LAST_READING "./tmp/reading.tmp"
#define MAX_ROW_LEN 100 /* Max length of row of file G18.txt */
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
 * - coord = Coordinate read from file 'reading.tmp'.
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

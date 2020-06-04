/* -------------------------------------------------------------------------- */
/* pfcs .c                                                                    */
/* -------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <uuid/uuid.h>
#include "log.h"
#include "mathgps.h"
#include "gpgll.h"
#include "pfcs.h"
#include "pfcs_util.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define DEBUG
#define MAX_ROW_LEN 100 /* Max length of row of file G18.txt */
#define MAX_ROW_LEN_LOG 200        /* Max length of log row */
#define UUID_LEN 37                /* Length of UUID */
#define GPGLL_LEN 45               /* Length of GPGLL */
#define READING_ROW_LEN UUID_LEN + MAX_ROW_LEN + 2
#define MAX_PFC_UNITS 3
#define READER_WAIT_TIME 1000      /* Millisecond */
#define PFC_WAIT_TIME 250          /* Millisecond */
#define LOG_CREATOR "PFC_CREATE"
#define LOG_READER "PFC_READER"
#define LOG_PFC_01 "PFC_01"
#define LOG_PFC_02 "PFC_02"
#define LOG_PFC_03 "PFC_03"
#define NUM_PFCS 4
#define MSG_CREATEPFCS_START_PROC "createPFCS - Start  proc: "
#define MSG_CREATEPFCS_FINISH_PROC "createPFCS - Finish proc: "
#define MSG_PFC_ERROR "ERROR - Error during the reading of file reading.tmp"
#define MSG_DISTANCE "Distance: "
#define MSG_VELOCITY "Velocity: "

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

#ifdef DEBUG
int main(int argc, char *argv[])
{
  createPFCS(argv[1]);
}
#endif

/*
 * DESCRIPTION
 * ...
 *
 * PARAMETERS:
 * ...
 *
 * RETURN VALUES:
 * ...
 */
void createPFCS(char *path)
{
  int pid_01, pid_02, pid_03, pid_reader;
  char tmpMsg[strlen(MSG_CREATEPFCS_START_PROC) + 20];

  init();

  /* Start PARENT */
  addLog(LOG_CREATOR, buildCreatePFCSStartMsg(tmpMsg, "PARENT"));
  pid_01 = fork();
  if (pid_01 == 0) {

    /* PFC 01 */
    addLog(LOG_CREATOR, buildCreatePFCSStartMsg(tmpMsg, "PFC 01"));
    pfc01();
    addLog(LOG_CREATOR, buildCreatePFCSFinishMsg(tmpMsg, "PFC 01"));
  } else {
    pid_02 = fork();
    if (pid_02 == 0) {

      /* PFC 02 */
      addLog(LOG_CREATOR, buildCreatePFCSStartMsg(tmpMsg, "PFC 02"));
      pfc02();
      addLog(LOG_CREATOR, buildCreatePFCSFinishMsg(tmpMsg, "PFC 02"));
    } else {
      pid_03 = fork();
      if (pid_03 == 0) {

        /* PFC 03 */
        addLog(LOG_CREATOR, buildCreatePFCSStartMsg(tmpMsg, "PFC 03"));
        pfc03();
        addLog(LOG_CREATOR, buildCreatePFCSFinishMsg(tmpMsg, "PFC 03"));
      } else {
        pid_reader = fork();
        if (pid_reader == 0) {

          /* READER */
          mssleep(200);
          addLog(LOG_CREATOR, buildCreatePFCSStartMsg(tmpMsg, "READER"));
          reader(path);
          addLog(LOG_CREATOR, buildCreatePFCSFinishMsg(tmpMsg, "READER"));
        } else {

          /* Finsh Parent */
          for (int i=0; i< NUM_PFCS; i++) {
            wait(NULL);
          }
          addLog(LOG_CREATOR, buildCreatePFCSFinishMsg(tmpMsg, "PARENT"));
        }
      }
    }
  }
}

/*
 * DESCRIPTION
 * ...
 */
void init(void)
{
  char tmpRdr[READING_ROW_LEN];
  writeCoord(buildReadingMsg(tmpRdr, "START"));
}

void pfc01(void)
{
  pfc(1);
}

void pfc02(void)
{
  pfc(2);
}

void pfc03(void)
{
pfc(3);
}

/*
 * DESCRIPTION
 * ...
 */
void pfc(int numPFC)
{
  char uuid_old[UUID_LEN] = "";
  char uuid_new[UUID_LEN] = "";
  char gpgll_A[GPGLL_LEN] = "";
  char gpgll_B[GPGLL_LEN] = "";
  char coord[MAX_ROW_LEN] = "";
  char logMsg[MAX_ROW_LEN_LOG] = "";
  double dDist = 0;
  double dVel = 0;

  while (1) {
    if (readCoord(coord) == NULL) {
      addLogPFC(numPFC, MSG_PFC_ERROR);
    } else {
      getUUID(uuid_new, coord);   /* Get UUID */
      getGPGLL(gpgll_B, coord);   /* Get GPGLL */
      if (strcmp("END", gpgll_B) == 0) {
        addLogPFC(numPFC, "END");
        break;
      }
      if (strcmp(uuid_old, uuid_new) != 0) {

        /* ACTION */
        if (strcmp("START",gpgll_B) != 0) {
          addLogPFC(numPFC, gpgll_A);
          addLogPFC(numPFC, gpgll_B);
          if (strlen(gpgll_A) == 44) {
            dDist = distanceBetweenEarthCoordinates(getLat(gpgll_A),
                                                    getLon(gpgll_A),
                                                    getLat(gpgll_B),
                                                    getLon(gpgll_B));
            sprintf(logMsg, "Dist: %f m\n", dDist);
            addLogPFC(numPFC, logMsg);
            dVel = dDist * 3.6;
            sprintf(logMsg, "Vel: %f km/h\n", dVel);
            addLogPFC(numPFC, logMsg);
          }
          strcpy(uuid_old, uuid_new);
          strcpy(gpgll_A, gpgll_B);
        }
      }
      mssleep(PFC_WAIT_TIME);
    }
  }
}

/*
 * DESCRIPTION
 * ...
 *
 * PARAMETERS:
 * - path = path of file G18.txt
 */
void reader(char *path)
{
  char coordinate[MAX_ROW_LEN];
  char tmpRdr[READING_ROW_LEN];

  if (GPGLLInit(path) == 0) {
    while (GPGLLPop(coordinate) != NULL) {
      writeCoord(buildReadingMsg(tmpRdr, coordinate));
      mssleep(READER_WAIT_TIME);
    }
    writeCoord(buildReadingMsg(tmpRdr, "END"));
  }
}


/*
 * DESCRIPTION
 * This function adds the value of the parameter msg into the log file identify
 * by parameter id.
 *
 * PARAMETERS:
 * - pfcNum = identifier of the process;
 * - msg = message to add.
 *
 * RETURN VALUES:
 * - 0 = Message added into the log file;
 * - 1 = Message too long;
 * - 2 = Occur error during opening file;
 * - 3 = Occur error during writing file;
 * - 4 = Unknown process.
 */
int addLogPFC(int pfcNum, char *msg)
{
  int res = 4;

  switch (pfcNum) {
  case 1:
    res = addLog(LOG_PFC_01, msg);
    break;
  case 2:
    res = addLog(LOG_PFC_02, msg);
    break;
  case 3:
    res = addLog(LOG_PFC_03, msg);
  }
  return res;
}

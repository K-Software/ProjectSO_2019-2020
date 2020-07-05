/* -------------------------------------------------------------------------- */
/* pfcs.c                                                                     */
/* -------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>
#include <uuid/uuid.h>
#include "common.h"
#include "log.h"
#include "file_channel.h"
#include "mathgps.h"
#include "gpgll.h"
#include "pfcs.h"
#include "pfcs_util.h"
#include "pipe_channel.h"
#include "socket_channel.h"
#include "string_util.h"
#include "time_util.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define MAX_ROW_LEN 100            /* Max length of row of file G18.txt */
#define GPGLL_LEN 45               /* Length of GPGLL */
#define READING_ROW_LEN UUID_LEN + MAX_ROW_LEN + 2
#define MAX_PFC_UNITS 3
#define READER_WAIT_TIME 1000      /* Millisecond */
#define PFC_WAIT_TIME 250          /* Millisecond */
#define DEFAULT_PROTOCOL 0
#define LOG_CREATOR "PFC_CREATE"
#define LOG_READER "PFC_READER"
#define LOG_PFC_01 "PFC_01"
#define LOG_PFC_02 "PFC_02"
#define LOG_PFC_03 "PFC_03"
#define MSG_CREATEPFCS_START_PROC "createPFCS - Start  proc: "
#define MSG_CREATEPFCS_FINISH_PROC "createPFCS - Finish proc: "
#define MSG_PFC_ERROR_READ_COORD "ERROR - Error during the reading of file reading.tmp"
#define MSG_PFC_ERROR_WRITE_VEL "ERROR - Error during writing the velocity"

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

  pfcsInit();

  /* Start PARENT */
  addLog(LOG_CREATOR, buildCreatePFCSStartMsg(tmpMsg, "PARENT"));
  pid_01 = fork();
  if (pid_01 == 0) {

    /* PFC 01 - PIPE CHANNEL */
    addLog(LOG_CREATOR, buildCreatePFCSStartMsg(tmpMsg, "PFC 01"));
    pfc01();
    addLog(LOG_CREATOR, buildCreatePFCSFinishMsg(tmpMsg, "PFC 01"));
  } else {
    pid_02 = fork();
    if (pid_02 == 0) {

      /* PFC 02 - SOCKET CHANNEL */
      addLog(LOG_CREATOR, buildCreatePFCSStartMsg(tmpMsg, "PFC 02"));
      pfc02();
      addLog(LOG_CREATOR, buildCreatePFCSFinishMsg(tmpMsg, "PFC 02"));
    } else {
      pid_03 = fork();
      if (pid_03 == 0) {

        /* PFC 03 - FILE CHANNEL */
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
 *
 */
void pfcsInit(void)
{
  char tmpRdr[READING_ROW_LEN];
  writeCoord(buildUuidMsg(tmpRdr, "START"));
}

/*
 * DESCRIPTION
 * This function creates a PFC that uses a pipe to communicate with TRANSDUCERS.
 *
 */
void pfc01(void)
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
      addLogPFC(1, MSG_PFC_ERROR_READ_COORD);
    } else {
      getUUID(uuid_new, coord);   /* Get UUID */
      getGPGLL(gpgll_B, coord);   /* Get GPGLL */
      if (strcmp("END", gpgll_B) == 0) {
        addLogPFC(1, "END");
        closePipeChnnlWrite();
        break;
      }
      if (strcmp(uuid_old, uuid_new) != 0) {

        /* ACTION */
        if (strcmp("START",gpgll_B) != 0) {
          addLogPFC(1, gpgll_A);
          addLogPFC(1, gpgll_B);
          if (strlen(gpgll_A) == 44) {
            dDist = distanceBetweenEarthCoordinates(getLat(gpgll_A),
                                                    getLon(gpgll_A),
                                                    getLat(gpgll_B),
                                                    getLon(gpgll_B));
            dVel = dDist * 3.6;
          } else {
            dDist = 0;
            dVel = 0;
          }
          sprintf(logMsg, "Dist: %f m - Vel: %f km/h\n", dDist, dVel);
          addLogPFC(1, logMsg);
          if (writePipeChnnl(dVel) > 0) {
            addLogPFC(1, MSG_PFC_ERROR_WRITE_VEL);
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
 * This function creates a PFC that uses a socket to communicate with
 * TRANSDUCERS.
 *
 */
void pfc02(void)
{
  char uuid_old[UUID_LEN] = "";
  char uuid_new[UUID_LEN] = "";
  char gpgll_A[GPGLL_LEN] = "";
  char gpgll_B[GPGLL_LEN] = "";
  char coord[MAX_ROW_LEN] = "";
  char logMsg[MAX_ROW_LEN_LOG] = "";
  double dDist = 0;
  double dVel = 0;
  int iServerFp, iClientFp, iServerLen;
  unsigned int iClientLen;

  struct sockaddr_un serverUNIXAddress;              /* Server address        */
  struct sockaddr* serverSockAddrPtr;                /* Ptr to server address */
  struct sockaddr_un clientUNIXAddress;              /* Client address        */
  struct sockaddr* clientSockAddrPtr;                /* Ptr to client address */

  serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
  iServerLen = sizeof (serverUNIXAddress);
  clientSockAddrPtr = (struct sockaddr*) &clientUNIXAddress;
  iClientLen = sizeof (clientUNIXAddress);

  iServerFp = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
  serverUNIXAddress.sun_family = AF_UNIX;            /* Set domain type       */
  strcpy (serverUNIXAddress.sun_path,
          PATH_SOCKET_CHANNEL);                      /* Set name              */
  unlink (PATH_SOCKET_CHANNEL);                      /* Remove file if it     */
                                                     /* already exists        */
  bind(iServerFp, serverSockAddrPtr,
       iServerLen);                                  /* Create file           */
  listen(iServerFp, 1);                              /* Maximum pending       */
                                                     /* connection length     */
  while (1) {
    iClientFp = accept(iServerFp, clientSockAddrPtr, &iClientLen);
    if (readCoord(coord) == NULL) {
      addLogPFC(2, MSG_PFC_ERROR_READ_COORD);
      close(iClientFp);
    } else {
      getUUID(uuid_new, coord);                      /* Get UUID              */
      getGPGLL(gpgll_B, coord);                      /* Get GPGLL             */
      if (strcmp("END", gpgll_B) == 0) {
        addLogPFC(2, "END");
        writeEndSocketChnnl(iClientFp);
        closeClientSocket(iClientFp);                /* Close client socket   */
        break;
      }
      if (strcmp(uuid_old, uuid_new) != 0) {

        /* ACTION */
        if (strcmp("START",gpgll_B) != 0) {
          addLogPFC(2, gpgll_A);
          addLogPFC(2, gpgll_B);
          if (strlen(gpgll_A) == 44) {
            dDist = distanceBetweenEarthCoordinates(getLat(gpgll_A),
                                                    getLon(gpgll_A),
                                                    getLat(gpgll_B),
                                                    getLon(gpgll_B));
            dVel = dDist * 3.6;
          } else {
            dDist = 0;
            dVel = 0;
          }
          sprintf(logMsg, "Dist: %f m - Vel: %f km/h\n", dDist, dVel);
          addLogPFC(2, logMsg);
          if (writeSocketChnnl(iClientFp, dVel) > 0) {
            addLogPFC(2, MSG_PFC_ERROR_WRITE_VEL);
          }
          closeClientSocket(iClientFp);                /* Close client socket */
          strcpy(uuid_old, uuid_new);
          strcpy(gpgll_A, gpgll_B);
        } else {
          closeClientSocket(iClientFp);                /* Close client socket */
        }
      } else {
        closeClientSocket(iClientFp);                  /* Close client socket */
      }
    }
  }
}

/*
 * DESCRIPTION
 * This function creates a PFC that uses a file to communicate with TRANSDUCERS.
 *
 */
void pfc03(void)
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
      addLogPFC(3, MSG_PFC_ERROR_READ_COORD);
    } else {
      getUUID(uuid_new, coord);   /* Get UUID */
      getGPGLL(gpgll_B, coord);   /* Get GPGLL */
      if (strcmp("END", gpgll_B) == 0) {
        addLogPFC(3, "END");
        closeFileChnnl();
        break;
      }
      if (strcmp(uuid_old, uuid_new) != 0) {

        /* ACTION */
        if (strcmp("START",gpgll_B) != 0) {
          addLogPFC(3, gpgll_A);
          addLogPFC(3, gpgll_B);
          if (strlen(gpgll_A) == 44) {
            dDist = distanceBetweenEarthCoordinates(getLat(gpgll_A),
                                                    getLon(gpgll_A),
                                                    getLat(gpgll_B),
                                                    getLon(gpgll_B));
            dVel = dDist * 3.6;
          } else {
            dDist = 0;
            dVel = 0;
          }
          sprintf(logMsg, "Dist: %f m - Vel: %f km/h\n", dDist, dVel);
          addLogPFC(3, logMsg);
          if (writeFileChnnl(dVel) > 0) {
            addLogPFC(3, MSG_PFC_ERROR_WRITE_VEL);
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
 * This function creates a specific process that read data from file G18.txt
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
      writeCoord(buildUuidMsg(tmpRdr, coordinate));
      mssleep(READER_WAIT_TIME);
    }
    writeCoord(buildUuidMsg(tmpRdr, "END"));
  }
}

/*
 * DESCRIPTION
 * This function adds the value of the parameter msg into the log file identify
 * by parameter id.
 *
 * PARAMETERS
 * - pfcNum = identifier of the process;
 * - msg = message to add.
 *
 * RETURN VALUES
 * Upon successful completion, addLogPFC return 0. Otherwise, return a value
 * different than 0.
 * If return 1 the message is too long.
 * If return 2 there was an error during opening file.
 * If return 3 there was an error during writing file.
 * If return 4 Unknown process.
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

/* -------------------------------------------------------------------------- */
/* transducers.c                                                              */
/* -------------------------------------------------------------------------- */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include "common.h"
#include "file_channel.h"
#include "failure_generator.h"
#include "log.h"
#include "pipe_channel.h"
#include "process_util.h"
#include "socket_channel.h"
#include "string_util.h"
#include "time_util.h"
#include "transducers.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define LOG_TRANSDUCERS "TRANSDUCERS"
#define DEFAULT_PROTOCOL 0

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

/*
 * DESCRIPTION
 * This function create the transducers.
 *
 */
void createTranducers(void)
{
  char lastUuidFile[UUID_LEN] = "START";
  char uuid[UUID_LEN] = "";
  char sPFC01Status[PFC_STATUS_LEN] = "";
  char sPFC02Status[PFC_STATUS_LEN] = "";
  char sPFC03Status[PFC_STATUS_LEN] = "";
  int iResStream1 = 0;
  int iResStream2 = 0;
  int iResStream3 = 0;
  int pp;                                                     /* Pipe pointer */
  int iClientFp;
  int iStatusIns = 0;
  int iStatusEnd = 0;
  int iPFCActive = 0;
  char testLog[MAX_ROW_LEN_LOG];

  /* START */
  addLog(LOG_TRANSDUCERS, "START");
  openPipeChnnlRead(&pp);                  /* Open pipe channel in read mode  */

  /* TODO: function to create socket */
  iClientFp = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);

  mssleep(200);
  while (1) {

    generateUuid(uuid);                                      /* Generate UUID */

    iResStream1 = readFromPipe(pp, uuid);                    /* PFC 01        */
    if (iResStream1 == 1) {
      iStatusEnd++;
    }

    iResStream2 = readFromSocket(iClientFp, uuid);           /* PFC 02        */
    if (iResStream2 == 1) {
      iStatusEnd++;
    }

    iResStream3 = readFromFile(lastUuidFile, uuid);          /* PFC 03        */
    if (iResStream3 == 1) {
      iStatusEnd++;
    }

    sprintf(testLog, "PIPE: %d, SOCKET: %d, FILE: %d",
            iResStream1, iResStream2, iResStream3);
    addLog(LOG_TRANSDUCERS, testLog);
    mssleep(300);
    if (iStatusEnd >= 3) {
      break;
    }
  }

  close(iClientFp);
  closePipeChnnlRead(pp);                  /* Close pipe channel in read mode */
  addLog(LOG_TRANSDUCERS, "END");
}

/*
 * DESCRIPTION
 * This function reads the velocity from the pipe channel and writes this
 * velocity in specified log file.
 *
 * PARAMETERS
 * - pipePointer = file pointer to read pipe.
 * - uuid = ...
 *
 * RETURN VALUES
 * Upon successful completion, readFromPipe returns 0. Otherwise, return a
 * value different than 0.
 * If return 1 the stream is ended.
 */
int readFromPipe(int pipePointer, char *uuid)
{
  char sVel[VELOCITY_VALUE_LEN] = "";
  char sLogVel[MAX_ROW_LEN_LOG] = "";
  char sLogMsg[5 + VELOCITY_VALUE_LEN] = "";
  char sLastUuidIns[UUID_LEN] = "";
  int iRes = 0;
  char sPid[10] = "";
  int iPid = 0;
  int status = 0;

  readPipeChnnl(pipePointer, sVel);
  if (strlen(sVel) > 0) {
    getSubStr(sVel, sVel, 0, strlen(sVel) -2);
    if (strcmp(sVel, "END") == 0) {
      iRes = 1;
      buildLogMsg(sLogVel, uuid, END_SPEED_STREAM);
      addLog(SPEED_PFC_01, sLogVel);                         /* End stream    */
    } else {
      sprintf(sLogMsg, "PIPE:%s", sVel);
      addLog(LOG_TRANSDUCERS, sLogMsg);
      buildLogMsg(sLogVel, uuid, sVel);
      addLog(SPEED_PFC_01, sLogVel);                         /* Log speed     */
    }
  } else {
    iRes = 2;
  }
  return iRes;
}

/*
 * DESCRIPTION
 * This function reads the velocity from the socket channel and writes this
 * velocity in specified log file.
 *
 * PARAMETERS
 * - uuid = ...
 *
 * RETURN VALUES
 * Upon successful completion, readFromSocket returns 0. Otherwise, return a
 * value different than 0.
 * If return 1 the stream is ended.
 * If return 2 generic error.
 */
int readFromSocket(int iClientFp, char *uuid)
{
  char sVel[VELOCITY_VALUE_LEN] = "";
  char sLogMsg[5 + VELOCITY_VALUE_LEN] = "";
  char sLogVel[MAX_ROW_LEN_LOG] = "";
  char sLastUuidIns[UUID_LEN] = "";
  int iServerLen, iResult;
  int n = 0;
  int iRes = 0;
  struct sockaddr_un serverUNIXAddress;              /* Server address        */
  struct sockaddr* serverSockAddrPtr;                /* Ptr to server address */

  serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
  iServerLen = sizeof (serverUNIXAddress);

  serverUNIXAddress.sun_family = AF_UNIX;                    /* Server domain */
  strcpy (serverUNIXAddress.sun_path, PATH_SOCKET_CHANNEL);  /* Server name   */

  do { /* Loop until a connection is made with the server */
    iResult = connect (iClientFp, serverSockAddrPtr, iServerLen);
    if (iResult == -1){
      addLog(LOG_TRANSDUCERS,
             "SOCKET - connection problem;re-try in 20 msec\n");
      mssleep (20); /* Wait and then try again */
    }
    n++;
  } while (iResult == -1 && n < 3);
  addLog(LOG_TRANSDUCERS, "SOCKET - connect to Server");
  readSocketChnnl(iClientFp, sVel);
  if (strlen(sVel) > 0) {
    getSubStr(sVel, sVel, 0, strlen(sVel) -2);
    if (strcmp(sVel, "END") == 0) {
      iRes = 1;
      buildLogMsg(sLogVel, uuid, END_SPEED_STREAM);
      addLog(SPEED_PFC_02, sLogVel);                         /* End stream    */
    } else {
      sprintf(sLogMsg, "SOCKET:%s", sVel);
      addLog(LOG_TRANSDUCERS, sLogMsg);
      buildLogMsg(sLogVel, uuid, sVel);
      addLog(SPEED_PFC_02, sLogVel);                         /* Log speed     */
    }
  } else {
    iRes = 2;
  }
  return iRes;
}

/*
 * DESCRIPTION
 * This function reads the velocity from the file channel and writes this
 * velocity in specified log file.
 *
 * PARAMETERS:
 * - previsUuid = UUID of last reader.
 * - uuid = ...
 *
 * RETURN VALUES:
 * Upon successful completion, readFromFile return 0. Otherwise, return a value
 * different than 0.
 * if return 1 the stream is ended.
 * If return 2 generic error.
 */
int readFromFile(char *previsUuid, char *uuid)
{
  char sVel[VELOCITY_VALUE_LEN] = "";
  char sTmpStr[UUID_LEN + VELOCITY_VALUE_LEN] = "";
  char sTmpUuid[UUID_LEN] = "";
  char sLastUuidIns[UUID_LEN] = "";
  char sLogMsg[5 + UUID_LEN + VELOCITY_VALUE_LEN] = "";
  char sLogVel[MAX_ROW_LEN_LOG] = "";
  int iRes = 0;
  int iResRead = 0;

  iResRead = readFileChnnl(sTmpStr);
  getUUID(sTmpUuid, sTmpStr);
  if ((iResRead == 0) && (strcmp(previsUuid, sTmpUuid) != 0)) {
    // New velocity
    sprintf(sLogMsg, "FILE:%s", sTmpStr);
    addLog(LOG_TRANSDUCERS, sLogMsg);
    strcpy(previsUuid, sTmpUuid);
    getSubStr(sTmpStr, sVel,UUID_LEN + 2, strlen(sTmpStr) - 1);
    if (strcmp(sVel, "END") == 0) {
      iRes = 1;
      buildLogMsg(sLogVel, uuid, END_SPEED_STREAM);
      addLog(SPEED_PFC_03, sLogVel);                         /* End stream    */
    } else {
      buildLogMsg(sLogVel, uuid, sVel);
      addLog(SPEED_PFC_03, sLogVel);                         /* Log speed     */
    }
  } else {
    iRes = 2;
  }
  return iRes;
}

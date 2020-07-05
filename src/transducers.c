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
#include "common.h"
#include "file_channel.h"
#include "log.h"
#include "pipe_channel.h"
#include "socket_channel.h"
#include "string_util.h"
#include "time_util.h"
#include "transducers.h"
#include "velocity_util.h"

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
  char lastUuidPipe[UUID_LEN] = "START";
  char lastUuidSocket[UUID_LEN] = "START";
  char lastUuidFile[UUID_LEN] = "START";
  int endStream1 = 0;
  int endStream2 = 0;
  int endStream3 = 0;
  int pp; /* Pipe pointer */

  /* START */
  addLog(LOG_TRANSDUCERS, "START");
  openPipeChnnlRead(&pp);                  /* Open pipe channel in read mode  */
  mssleep(200);
  while (1) {
    endStream1 = readFromPipe(pp);                            /* PFC 01       */

    endStream2 = readFromSocket();                            /* PFC 02       */

    endStream3 = readFromFile(lastUuidFile);                  /* PFC 03       */
    mssleep(200);
    if (endStream1 || endStream2 || endStream3) {
      break;
    }
  }
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
 *
 * RETURN VALUES
 * Upon successful completion, readFromPipe returns 0. Otherwise, return a
 * value different than 0.
 * If return 1 the stream is ended.
 */
int readFromPipe(int pipePointer)
{
  char sVel[VELOCITY_VALUE_LEN] = "";
  char sLogMsg[5 + VELOCITY_VALUE_LEN] = "";
  int iRes = 0;

  readPipeChnnl(pipePointer, sVel);
  if (strlen(sVel) > 0) {
    getSubStr(sVel, sVel, 0, strlen(sVel) -2);
    if (strcmp(sVel, "END") == 0) {
      iRes = 1;                                              /* End stream    */
    } else {
      sprintf(sLogMsg, "PIPE:%s", sVel);
      addLog(LOG_TRANSDUCERS, sLogMsg);
      addLog(SPEED_PFC_01, sVel);
    }
  }
  return iRes;
}

/*
 * DESCRIPTION
 * This function reads the velocity from the socket channel and writes this
 * velocity in specified log file.
 *
 * RETURN VALUES
 * Upon successful completion, readFromSocket returns 0. Otherwise, return a
 * value different than 0.
 * If return 1 the stream is ended.
 */
int readFromSocket(void)
{
  char sVel[VELOCITY_VALUE_LEN] = "";
  char sLogMsg[5 + VELOCITY_VALUE_LEN] = "";
  int iClientFp, iServerLen, iResult;
  int n = 0;
  int iRes = 0;
  struct sockaddr_un serverUNIXAddress;              /* Server address        */
  struct sockaddr* serverSockAddrPtr;                /* Ptr to server address */

  serverSockAddrPtr = (struct sockaddr*) &serverUNIXAddress;
  iServerLen = sizeof (serverUNIXAddress);

  iClientFp = socket (AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
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
      iRes = 1;                                              /* End stream    */
    } else {
      sprintf(sLogMsg, "SOCKET:%s", sVel);
      addLog(LOG_TRANSDUCERS, sLogMsg);
      addLog(SPEED_PFC_02, sVel);
    }
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
 *
 * RETURN VALUES:
 * Upon successful completion, readFromFile return 0. Otherwise, return a value
 * different than 0.
 * if return 1 the stream is ended.
 */
int readFromFile(char *previsUuid)
{
  char sVel[VELOCITY_VALUE_LEN] = "";
  char sTmpStr[UUID_LEN + VELOCITY_VALUE_LEN] = "";
  char sTmpUuid[UUID_LEN] = "";
  char sLogMsg[5 + UUID_LEN + VELOCITY_VALUE_LEN] = "";
  int iRes = 0;
  int iResRead = 0;

  iResRead = readFileChnnl(sTmpStr);
  getUUID(sTmpUuid, sTmpStr);
  if ((iResRead == 0) && (strcmp(previsUuid, sTmpUuid) != 0)) {
    // New velocity
    sprintf(sLogMsg, "FILE:%s", sTmpStr);
    addLog(LOG_TRANSDUCERS, sLogMsg);
    strcpy(previsUuid, sTmpUuid);
    getSubStr(sTmpStr, sVel,UUID_LEN + 1, strlen(sTmpStr) - 1);
    if (strcmp(sVel, "END") == 0) {
      // End stream
      iRes = 1;
    } else {
      // Log new velocity
      addLog(SPEED_PFC_03, sVel);
    }
  }
  return iRes;
}

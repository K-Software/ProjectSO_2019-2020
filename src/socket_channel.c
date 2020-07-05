/* -------------------------------------------------------------------------- */
/* socket_channel.c                                                           */
/* -------------------------------------------------------------------------- */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>
#include "common.h"
#include "log.h"
#include "time_util.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define DEFAULT_PROTOCOL 0
#define LOG_SOCKET "CHANNEL_SOCKET"
#define MSG_LOG_SOCKET_WRITE_VELOCITY "channel socket - Write velocity: "
#define MSG_LOG_SOCKET_READ_VELOCITY "channel socket - Read velocity: "
#define MSG_LOG_SOCKET_CLOSE_CLIENT "channel socket - Close client socket"
#define MSG_LOG_SOCKET_ERR_WRITE_TMP "channel socket - Error writing socket"
#define MSG_LOG_SOCKET_ERR_READ_TMP "channel socket - Error reading socket"
#define MSG_LOG_SOCKET_ERR_CLOSE_CLIENT "channel socket - Error closing client socket"

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

/*
 * DESCRIPTION
 * This function reads the velocity from socket socket_vel.tmp.
 *
 * PARAMETERS
 * iClientFp = File pointer of client socket
 * sVel = Velocity
 *
 * RETURN VALUES
 * Upon successful completion, readSocketChannl returns 0. Otherwise,
 * return a value different than 0.
 * If return 1 there was an error during reading velocity from the server.
 */
int readSocketChnnl(int iClientFp, char *sVel)
{
  char logMsg[MAX_ROW_LEN_LOG] = "";
  int n;
  char *sTemp = sVel;

  do {                           /* Read characters until ’#’ or end-of-input */
    n = read (iClientFp, sVel, 1);                      /* Read one character */
  } while (n > 0 && *sVel++ != '#');
  if (n == -1) {
    addLog(LOG_SOCKET, MSG_LOG_SOCKET_ERR_READ_TMP);
    return 1;
  }
  strcpy(logMsg, MSG_LOG_SOCKET_READ_VELOCITY);
  strcat(logMsg, sTemp);
  addLog(LOG_SOCKET, logMsg);
  return 0;
}

/*
 * DESCRIPTION
 * This function writes velocity in socket socket_vel.tmp
 *
 * PARAMETERS
 * iClientFp = File pointer of client socket
 * dVel = Velocity
 *
 * RETURN
 * Upon successful completion, writeSocketChnnl returns 0. Otherwise, return a
 * value different than 0.
 * If return 1 there was an error during writing on sucket.
 */
int writeSocketChnnl(int iClientFp, double dVel)
{
  char logMsg[MAX_ROW_LEN_LOG] = "";
  char sVel[VELOCITY_VALUE_LEN + 1] = "0";

  sprintf(sVel, "%f", dVel);
  strcat(sVel, "#");
  if (write(iClientFp, sVel, strlen(sVel)) == -1) {        /* Write in socket */
    addLog(LOG_SOCKET, MSG_LOG_SOCKET_ERR_WRITE_TMP);
    return 1;
  }
  strcpy(logMsg, MSG_LOG_SOCKET_WRITE_VELOCITY);
  strcat(logMsg, sVel);
  addLog(LOG_SOCKET, logMsg);
  return 0;
}

/*
 * DESCRIPTION
 * This function writes the value END in socket socket_vel.tmp
 *
 * PARAMETERS
 * iClientFp = File pointer of client socket
 *
 * RETURN VALUES
 * Upon successful completion, writeEndSocketChnnl returns 0. Otherwise, return
 * a value different than 0.
 * If return 1 there was an error during writing on sucket.
 */
int writeEndSocketChnnl(int iClientFp)
{
  char logMsg[MAX_ROW_LEN_LOG] = "";
  char *sVel = "END#";

  if (write(iClientFp, sVel, strlen(sVel)) == -1) {          /* Write in pipe */
    addLog(LOG_SOCKET, MSG_LOG_SOCKET_ERR_WRITE_TMP);
    return 1;
  }
  strcpy(logMsg, MSG_LOG_SOCKET_WRITE_VELOCITY);
  strcat(logMsg, sVel);
  addLog(LOG_SOCKET, logMsg);
  return 0;
}

/*
 * DESCRIPTION
 * This function is specific to close the client socket.
 *
 * PARAMETERS
 * iClientFp = File pointer of client socket
 *
 * RETURN VALUES
 * Upon successful completion, closeClientSocket returns 0. Otherwise, return
 * a value different than 0.
 * If return 1 there was an error during the closing of sucket.
 */
int closeClientSocket(int iClientFp)
{
  if (close(iClientFp) != 0) {                               /*  Close socket */
    addLog(LOG_SOCKET, MSG_LOG_SOCKET_ERR_CLOSE_CLIENT);
    return 1;
  }
  addLog(LOG_SOCKET, MSG_LOG_SOCKET_CLOSE_CLIENT);
  return 0;
}

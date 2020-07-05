/* -------------------------------------------------------------------------- */
/* pipe_channel.c                                                             */
/* -------------------------------------------------------------------------- */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include "common.h"
#include "log.h"
#include "string_util.h"
#include "time_util.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define LOG_PIPE "CHANNEL_PIPE"
#define MSG_LOG_PIPE_OPEN_PIPE_READ_TMP "channel pipe read  - Open temporary pipe"
#define MSG_LOG_PIPE_CLOSE_PIPE_READ_TMP "channel pipe read  - Close temporary pipe"
#define MSG_LOG_PIPE_OPEN_PIPE_WRITE_TMP "channel pipe write - Open temporary pipe"
#define MSG_LOG_PIPE_CLOSE_PIPE_WRITE_TMP "channel pipe write - Close temporary pipe"
#define MSG_LOG_PIPE_WRITE_VELOCITY "channel pipe write - Write velocity: "
#define MSG_LOG_PIPE_READ_VELOCITY "channel pipe read  - Read velocity: "
#define MSG_LOG_PIPE_UNLINK_PIPE_TMP "channel pipe - Unlink temporary pipe"
#define MSG_LOG_PIPE_ERR_OPEN_PIPE_READ_TMP "channel pipe read  - Error opening temporary pipe"
#define MSG_LOG_PIPE_ERR_OPEN_PIPE_WRITE_TMP "channel pipe write - Error opening temporaty pipe"
#define MSG_LOG_PIPE_ERR_WRITE_PIPE_TMP "channel pipe - Error writing temporary pipe"
#define MSG_LOG_PIPE_ERR_READ_PIPE_TMP "channel pipe - Error reading temporary pipe"
#define MSG_LOG_PIPE_ERR_CLOSE_PIPE_READ_TMP "channel pipe read  - Error closing temporary pipe"
#define MSG_LOG_PIPE_ERR_CLOSE_PIPE_WRITE_TMP "channel pipe write - Error closing temporary pipe"
#define MSG_LOG_PIPE_ERR_UNLINK_PIPE_TMP "channel pipe - Error unlink temporary pipe"

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

/*
 * DESCRIPTION
 * This function opens a pipe in read mode.
 *
 * PARAMETERS
 * - pipePointer = file pointer of pipe.
 *
 * RETURN VALUES
 * Upon successful completion, openPipeChnl returns 0. Otherwise, return a
 * value different than 0.
 * If return 1 there was an error during opening the pipe.
 */
int openPipeChnnlRead(int *pipePointer)
{
  int iRes = 0;

  *pipePointer = open(PATH_PIPE_CHANNEL,
                      O_RDONLY | O_NONBLOCK);       /* Open pipe in read mode */
  if (*pipePointer > -1) {
    addLog(LOG_PIPE, MSG_LOG_PIPE_OPEN_PIPE_READ_TMP);
  } else {
    addLog(LOG_PIPE, MSG_LOG_PIPE_ERR_OPEN_PIPE_READ_TMP);
    iRes = 1;
  }
  return iRes;
}

/*
 * DESCRIPTION
 * This function reads velocity from pipe.
 *
 * PARAMETERS
 * - pipePointer = File pointer of pipe
 * - sVel = Velocity
 *
 * RETURN VALUES:
 * ...
 */
int readPipeChnnl(int pipePointer, char *sVel)
{
  int n;
  char logMsg[MAX_ROW_LEN_LOG] = "";
  char *sTemp = sVel;

  do {
    n = read(pipePointer, sVel, 1);
  } while (n > 0 && *sVel++ != '#');
  sprintf(logMsg, "%s %s", MSG_LOG_PIPE_READ_VELOCITY, sTemp);
  addLog(LOG_PIPE, logMsg);
  return (n > 0);
}

/*
 * DESCRIPTION
 * This function writes the velocity in the pipe.
 *
 * PARAMETERS
 * - dVel = Velocity
 *
 * RETURN VALUES
 * Upon successful completion, writePipeChannl returns 0. Otherwise, return a
 * value different than 0.
 * If return 1 there was an error during opening the pipe.
 * If return 2 there was an error during writing the velocity in the pipe.
 * If return 3 there was an error during closing the pipe.
 */
int writePipeChnnl(double dVel)
{
  int pp;
  int iCounter = 0;
  char logMsg[MAX_ROW_LEN_LOG] = "";
  char sVel[VELOCITY_VALUE_LEN + 1] = "0";

  do {
    pp = open(PATH_PIPE_CHANNEL, O_WRONLY);        /* Open pipe in write mode */
    if (pp == -1) mssleep(10);
    iCounter++;
  } while (pp == -1 && iCounter < 5);
  if (pp == -1) {
    printf("%s", strerror(errno));
    addLog(LOG_PIPE, MSG_LOG_PIPE_ERR_OPEN_PIPE_WRITE_TMP);
    return 1;
  }
  addLog(LOG_PIPE, MSG_LOG_PIPE_OPEN_PIPE_WRITE_TMP);
  sprintf(sVel, "%f", dVel);
  strcat(sVel, "#");
  if (write(pp, sVel, strlen(sVel)) == -1) {       /* Write in pipe           */
    addLog(LOG_PIPE, MSG_LOG_PIPE_ERR_WRITE_PIPE_TMP);
    return 2;
  }
  strcpy(logMsg, MSG_LOG_PIPE_WRITE_VELOCITY);
  strcat(logMsg, sVel);
  addLog(LOG_PIPE, logMsg);
  if (close(pp) == -1) {                           /* Close pipe              */
    addLog(LOG_PIPE, MSG_LOG_PIPE_ERR_CLOSE_PIPE_WRITE_TMP);
    return 3;
  }
  addLog(LOG_PIPE, MSG_LOG_PIPE_CLOSE_PIPE_WRITE_TMP);
  return 0;
}

/*
 *DESCRIPTION
 * This function writes END# in the pipe.
 *
 * PARAMETERS
 * No parameters
 *
 * RETURN VALUES
 * Upon successful completion, closePipeChnnlWrite returns 0. Otherwise, return
 * a value different than 0.
 * If return 1 there was an error during opening the pipe.
 * If return 2 there was an error during writing the velocity in the pipe.
 * If return 3 there was an error during closing the pipe.
 */
int closePipeChnnlWrite(void)
{
  int pp;
  char logMsg[MAX_ROW_LEN_LOG] = "";

  pp = open(PATH_PIPE_CHANNEL, O_WRONLY);          /* Open pipe in write mode */
  if (pp == -1) {
    printf("%s", strerror(errno));
    addLog(LOG_PIPE, MSG_LOG_PIPE_ERR_OPEN_PIPE_WRITE_TMP);
    return 1;
  }
  addLog(LOG_PIPE, MSG_LOG_PIPE_OPEN_PIPE_WRITE_TMP);
  if (write(pp, "END#", strlen("END#")) == -1) {   /* Write in pipe           */
    addLog(LOG_PIPE, MSG_LOG_PIPE_ERR_WRITE_PIPE_TMP);
    return 2;
  }
  strcpy(logMsg, MSG_LOG_PIPE_WRITE_VELOCITY);
  strcat(logMsg, "END#");
  addLog(LOG_PIPE, logMsg);
  if (close(pp) == -1) {                           /* Close pipe              */
    addLog(LOG_PIPE, MSG_LOG_PIPE_ERR_CLOSE_PIPE_WRITE_TMP);
    return 3;
  }
  addLog(LOG_PIPE, MSG_LOG_PIPE_CLOSE_PIPE_WRITE_TMP);
  return 0;
}

/*
 * DESCRIPTION
 * This function closes the read pipe.
 *
 * PARAMETERS
 * - pipePointer = file pointer of pipe
 *
 * RETURN VALUES
 * Upon successful completion, closePipeChnl returns 0. Otherwise, return a
 * value different than 0.
 * If return 1 there was an error during closing the pipe.
 * if return 2 there was an error during unlinking the pipe.
 */
int closePipeChnnlRead(int pipePointer)
{
  int iRes = 0;

  if (close(pipePointer) == 0) {
    addLog(LOG_PIPE, MSG_LOG_PIPE_CLOSE_PIPE_READ_TMP);
  } else {
    addLog(LOG_PIPE, MSG_LOG_PIPE_ERR_CLOSE_PIPE_READ_TMP);
    iRes = 1;
  }
  if (unlink(PATH_PIPE_CHANNEL) == 0) {
    addLog(LOG_PIPE, MSG_LOG_PIPE_UNLINK_PIPE_TMP);
  } else {
    addLog(LOG_PIPE, MSG_LOG_PIPE_ERR_UNLINK_PIPE_TMP);
    iRes = 2;
  }
  return iRes;
}

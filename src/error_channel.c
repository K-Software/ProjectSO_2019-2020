/* -------------------------------------------------------------------------- */
/* error_channel.c                                                            */
/* -------------------------------------------------------------------------- */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <fcntl.h>
#include "common.h"
#include "error_channel.h"
#include "log.h"
#include "string_util.h"
#include "time_util.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define LOG_PIPE "ERROR_PIPE"
#define MSG_LOG_PIPE_OPEN_PIPE_READ_TMP "error pipe read  - Open temporary pipe"
#define MSG_LOG_PIPE_CLOSE_PIPE_READ_TMP "error pipe read  - Close temporary pipe"
#define MSG_LOG_PIPE_OPEN_PIPE_WRITE_TMP "error pipe write - Open temporary pipe"
#define MSG_LOG_PIPE_CLOSE_PIPE_WRITE_TMP "error pipe write - Close temporary pipe"
#define MSG_LOG_PIPE_WRITE_ERROR "error pipe write - Write error: "
#define MSG_LOG_PIPE_READ_ERROR "error pipe read  - Read error: "
#define MSG_LOG_PIPE_UNLINK_PIPE_TMP "error pipe - Unlink temporary pipe"
#define MSG_LOG_PIPE_ERR_OPEN_PIPE_READ_TMP "error pipe read  - Error opening temporary pipe"
#define MSG_LOG_PIPE_ERR_OPEN_PIPE_WRITE_TMP "error pipe write - Error opening temporaty pipe"
#define MSG_LOG_PIPE_ERR_WRITE_PIPE_TMP "error pipe - Error writing temporary pipe"
#define MSG_LOG_PIPE_ERR_READ_PIPE_TMP "error pipe - Error reading temporary pipe"
#define MSG_LOG_PIPE_ERR_CLOSE_PIPE_READ_TMP "error pipe read  - Error closing temporary pipe"
#define MSG_LOG_PIPE_ERR_CLOSE_PIPE_WRITE_TMP "error pipe write - Error closing temporary pipe"
#define MSG_LOG_PIPE_ERR_UNLINK_PIPE_TMP "error pipe - Error unlink temporary pipe"
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
 * Upon successful completion, openErrChnlRead returns 0. Otherwise, return a
 * value different than 0.
 * If return 1 there was an error during opening the pipe.
 */
int openErrChnnlRead(int *pipePointer)
{
  int iRes = 0;

  *pipePointer = open(PATH_ERROR_CHANNEL,
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
 * This function reads error from pipe.
 *
 * PARAMETERS
 * - pipePointer = File pointer of pipe
 * - sErr = Error
 *
 * RETURN VALUES
 * Upon successful completion, writeErrChannl returns 1. Otherwise, return a
 * value different than 1.
 */
int readErrChnnl(int pipePointer, char *sErr)
{
  int n;
  char logMsg[MAX_ROW_LEN_LOG] = "";
  char *sTemp = sErr;

  do {
    n = read(pipePointer, sErr, 1);
  } while (n > 0 && *sErr++ != '#');
  sprintf(logMsg, "%s %s", MSG_LOG_PIPE_READ_ERROR, sTemp);
  addLog(LOG_PIPE, logMsg);
  return (n > 0);
}

/*
 * DESCRIPTION
 * This function writes the error in the pipe.
 *
 * PARAMETERS
 * - sErr = Error
 *
 * RETURN VALUES
 * Upon successful completion, writeErrChannl returns 0. Otherwise, return a
 * value different than 0.
 * If return 1 there was an error during opening the pipe.
 * If return 2 there was an error during writing the error in the pipe.
 * If return 3 there was an error during closing the pipe.
 */
int writeErrChnnl(char *sErr)
{
  int pp;
  int iCounter = 0;
  char logMsg[MAX_ROW_LEN_LOG] = "";
  char sTmp[ERROR_VALUE_LEN + 1] = "";

  do {
    pp = open(PATH_ERROR_CHANNEL, O_WRONLY);       /* Open pipe in write mode */
    if (pp == -1) mssleep(10);
    iCounter++;
  } while (pp == -1 && iCounter < 5);
  if (pp == -1) {
    printf("%s", strerror(errno));
    addLog(LOG_PIPE, MSG_LOG_PIPE_ERR_OPEN_PIPE_WRITE_TMP);
    return 1;
  }
  addLog(LOG_PIPE, MSG_LOG_PIPE_OPEN_PIPE_WRITE_TMP);
  strcpy(sTmp, sErr);
  strcat(sTmp, "#");
  if (write(pp, sTmp, strlen(sTmp)) == -1) {       /* Write in pipe           */
    addLog(LOG_PIPE, MSG_LOG_PIPE_ERR_WRITE_PIPE_TMP);
    return 2;
  }
  strcpy(logMsg, MSG_LOG_PIPE_WRITE_ERROR);
  strcat(logMsg, sErr);
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
int closeErrChnnlRead(int pipePointer)
{
  int iRes = 0;

  if (close(pipePointer) == 0) {
    addLog(LOG_PIPE, MSG_LOG_PIPE_CLOSE_PIPE_READ_TMP);
  } else {
    addLog(LOG_PIPE, MSG_LOG_PIPE_ERR_CLOSE_PIPE_READ_TMP);
    iRes = 1;
  }
  if (unlink(PATH_ERROR_CHANNEL) == 0) {
    addLog(LOG_PIPE, MSG_LOG_PIPE_UNLINK_PIPE_TMP);
  } else {
    addLog(LOG_PIPE, MSG_LOG_PIPE_ERR_UNLINK_PIPE_TMP);
    iRes = 2;
  }
  return iRes;
}

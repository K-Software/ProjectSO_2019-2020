/* -------------------------------------------------------------------------- */
/* file_channel.c                                                             */
/* -------------------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "common.h"
#include "log.h"
#include "string_util.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define LOG_FILE "CHANNEL_FILE"
#define MSG_LOG_FILE_WRITE_VELOCITY "channel file - Write velocity: "
#define MSG_LOG_FILE_READ_VELOCITY "channel file - Read velocity: "
#define MSG_LOG_FILE_CLOSE "channel file - Close"
#define MSG_LOG_FILE_ERR_OPEN_FILE_TMP "channel file - Error opening temporary file"
#define MSG_LOG_FILE_ERR_WRITE_FILE_TMP "channel file - Error writing temporary file"
#define MSG_LOG_FILE_ERR_READ_FILE_TMP "channel file - Error reading temporary file"
#define MSG_LOG_FILE_ERR_CLOSE_FILE_TMP "channel file - Error closing temporary file"

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

/*
 * DESCRIPTION:
 * This function writes velocity in temporary file file_vel.tmp.
 *
 * PARAMETERS:
 * - dVel = Velocity
 *
 * RETURN VALUES:
 * Upon successful completion, writeFileChnl returns 0. Otherwise, return a
 * value different than 0.
 * If return 1 there was an error during opening the file.
 * If return 2 there was an error during writing the file.
 * If return 3 there was an error during closing the file.
 */
int writeFileChnnl(double dVel)
{
  FILE *fpTmp;
  char logMsg[MAX_ROW_LEN_LOG] = "";
  char sVel[VELOCITY_VALUE_LEN] = "0";
  char tmpWrt[UUID_LEN + VELOCITY_VALUE_LEN] = "";

  fpTmp = fopen(PATH_FILE_CHANNEL, "w");              /* Open temporary file  */
                                                      /* in write mode        */
  if (fpTmp == NULL) {
    addLog(LOG_FILE, MSG_LOG_FILE_ERR_OPEN_FILE_TMP);
    return 1;
  }
  sprintf(sVel, "%f", dVel);
  if (fputs(buildUuidMsg(tmpWrt,sVel), fpTmp) == EOF) {
    addLog(LOG_FILE, MSG_LOG_FILE_WRITE_VELOCITY);
    return 2;
  }
  if (fclose(fpTmp) != 0) {                           /* Close temporary file */
    addLog(LOG_FILE, MSG_LOG_FILE_ERR_CLOSE_FILE_TMP);
    return 3;
  }
  sprintf(logMsg, "%s %s", MSG_LOG_FILE_WRITE_VELOCITY, tmpWrt);
  addLog(LOG_FILE, logMsg);
  return 0;
}

/*
 * DESCRIPTION:
 * This function writes END in temporary file velocity.tmp.
 *
 * RETURN VALUES:
 * Upon successful completion, closeFileChnl returns 0. Otherwise, return a
 * value different than 0.
 * If return 1 there was an error during opening the file.
 * If return 2 there was an error during writing the file.
 * If return 3 there was an error during closing the file.
 */
int closeFileChnnl(void)
{
  FILE *fpTmp;
  int res = 0;
  char logMsg[MAX_ROW_LEN_LOG] = "";
  char tmpWrt[UUID_LEN + VELOCITY_VALUE_LEN] = "";

  fpTmp = fopen(PATH_FILE_CHANNEL, "w");              /* Open temporary file  */
                                                      /* in write mode        */
  if (fpTmp == NULL) {
    addLog(LOG_FILE, MSG_LOG_FILE_ERR_OPEN_FILE_TMP);
    return 1;
  }
  if (fputs(buildUuidMsg(tmpWrt,"END"), fpTmp) == EOF) {
    addLog(LOG_FILE, MSG_LOG_FILE_CLOSE);
    return 2;
  };
  if (fclose(fpTmp) != 0) {                           /* Close temporary file */
    addLog(LOG_FILE, MSG_LOG_FILE_ERR_CLOSE_FILE_TMP);
    return 3;
  }
  sprintf(logMsg, "%s %s", MSG_LOG_FILE_CLOSE, tmpWrt);
  addLog(LOG_FILE, logMsg);
  return res;
}

/*
 * DESCRIPTION
 * This function reads the velocity from temporary file velocity.tmp.
 *
 * PARAMETERS:
 * - vel = The velocity read from temporary file
 *
 * RETURN VALUES:
 * Upon successful completion, readFileChnl returns 0. Otherwise, return a
 * value different than 0.
 * If return 1 there was an error during opening the file.
 * If return 2 there was an error during reading the file.
 * If return 3 there was an error during closing the file.
 */
int readFileChnnl(char *vel)
{
  FILE *fpTmp;
  int res = 0;
  char logMsg[MAX_ROW_LEN_LOG] = "";

  fpTmp = fopen(PATH_FILE_CHANNEL, "r");              /* Open temporary file  */
                                                      /* in write mode        */
  if (fpTmp == NULL) {
    addLog(LOG_FILE, MSG_LOG_FILE_ERR_OPEN_FILE_TMP);
    return 1;
  }
  if (fgets(vel, 100, fpTmp) == NULL) {
    addLog(LOG_FILE, MSG_LOG_FILE_ERR_READ_FILE_TMP);
    return 2;
  };
  if (fclose(fpTmp) != 0) {                           /* Close temporary file */
    addLog(LOG_FILE, MSG_LOG_FILE_ERR_CLOSE_FILE_TMP);
    return 3;
  }
  sprintf(logMsg, "%s %s", MSG_LOG_FILE_READ_VELOCITY, vel);
  addLog(LOG_FILE, logMsg);
  return res;
}

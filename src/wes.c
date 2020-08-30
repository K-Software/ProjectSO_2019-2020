/* -------------------------------------------------------------------------- */
/* wes.c                                                                      */
/* -------------------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/errno.h>
#include "common.h"
#include "log.h"
#include "string_util.h"
#include "time_util.h"
#include "wes.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define LOG_WES "WES"
#define MSG_EXIST_SPEED_PFC_01 "createWES - Exist file speed_PFC_01"
#define MSG_EXIST_SPEED_PFC_02 "createWES - Exist file speed_PFC_02"
#define MSG_EXIST_SPEED_PFC_03 "createWES - Exist file speed_PFC_03"
#define MSG_LOG_PFC01_READING "UUID 01: %s - SPEED 01: %s"
#define MSG_LOG_PFC02_READING "UUID 02: %s - SPEED 02: %s"
#define MSG_LOG_PFC03_READING "UUID 03: %s - SPEED 03: %s"
#define MSG_LOG_STATUS_OK "%s - STATUS: OK        - (%s, %s, %s)"
#define MSG_LOG_STATUS_ERROR "%s - STATUS: ERROR     - (%s, %s, %s)"
#define MSG_LOG_STATUS_EMERGENCY "%s - STATUS: EMERGENCY - (%s, %s, %s)"
#define MSG_OUTPUT_STATUS_OK "%s - STATUS: OK        - (%s, %s, %s) \n"
#define MSG_OUTPUT_STATUS_ERROR "%s - STATUS: ERROR     - (%s, %s, %s) \n"
#define MSG_OUTPUT_STATUS_EMERGENCY "%s - STATUS: EMERGENCY - (%s, %s, %s) \n"

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

/*
 * DESCRIPTION
 * This function create the WES.
 *
 */
void createWES(void)
{
  FILE *fpSpeedPFC01, *fpSpeedPFC02, *fpSpeedPFC03;
  int iExistSpeed01 = 0;
  int iExistSpeed02 = 0;
  int iExistSpeed03 = 0;
  int iEndSpeed01 = 0;
  int iEndSpeed02 = 0;
  int iEndSpeed03 = 0;
  char sLogSpeed[MAX_ROW_LEN_LOG] = "";
  char sLogWES[MAX_ROW_LEN_LOG] = "";
  char sUuid[UUID_LEN] = "";
  char sUuid01[UUID_LEN] = "";
  char sUuid02[UUID_LEN] = "";
  char sUuid03[UUID_LEN] = "";
  char sSpeed01[VELOCITY_VALUE_LEN] = "";
  char sSpeed02[VELOCITY_VALUE_LEN] = "";
  char sSpeed03[VELOCITY_VALUE_LEN] = "";

  mssleep(2000);

  while (1) {
    if (access(buildFileName(SPEED_PFC_01), F_OK ) != -1 ) {
      addLog(LOG_WES, MSG_EXIST_SPEED_PFC_01);
      iExistSpeed01 = 1;
    }
    if (access(buildFileName(SPEED_PFC_02), F_OK ) != -1 ) {
      addLog(LOG_WES, MSG_EXIST_SPEED_PFC_02);
      iExistSpeed02 = 1;
    }
    if (access(buildFileName(SPEED_PFC_03), F_OK ) != -1 ) {
      addLog(LOG_WES, MSG_EXIST_SPEED_PFC_03);
      iExistSpeed03 = 1;
    }

    if (iExistSpeed01 && iExistSpeed02 && iExistSpeed03) {
      break;
    }
    mssleep(100);
  }

  while (1) {
    generateUuid(sUuid);                                     /* Generate UUID */

    /* START - SPEED PFC 01  */
    strcpy(sLogSpeed, "");
    strcpy(sLogWES, "");
    if (readSpeed(buildFileName(SPEED_PFC_01), sLogSpeed) == 0) {
      getSubStr(sLogSpeed, sUuid01, 24, 23 + UUID_LEN);
      getSubStr(sLogSpeed, sSpeed01, 26 + UUID_LEN, strlen(sLogSpeed) - 2);
      if (strcmp(sSpeed01, END_SPEED_STREAM) == 0)
        iEndSpeed01 = 1;
    } else {
      strcpy(sUuid01, "ERR");
      strcpy(sSpeed01, "ERR PFC 01");
    }
    sprintf(sLogWES, MSG_LOG_PFC01_READING, sUuid01, sSpeed01);
    addLog(LOG_WES, sLogWES);
    /* END   - SPPED PFC 01  */

    /* START - SPEED PFC 02  */
    strcpy(sLogSpeed, "");
    strcpy(sLogWES, "");
    if (readSpeed(buildFileName(SPEED_PFC_02), sLogSpeed) == 0) {
      getSubStr(sLogSpeed, sUuid02, 24, 23 + UUID_LEN);
      getSubStr(sLogSpeed, sSpeed02, 26 + UUID_LEN, strlen(sLogSpeed) - 2);
      if (strcmp(sSpeed02, END_SPEED_STREAM) == 0)
        iEndSpeed02 = 1;
    } else {
      strcpy(sUuid02, "ERR");
      strcpy(sSpeed02, "ERR PFC 02");
    }
    sprintf(sLogWES, MSG_LOG_PFC02_READING, sUuid02, sSpeed02);
    addLog(LOG_WES, sLogWES);
    /* END   - SPPED PFC 02  */

    /* START - SPEED PFC 03  */
    strcpy(sLogSpeed, "");
    strcpy(sLogWES, "");
    if (readSpeed(buildFileName(SPEED_PFC_03), sLogSpeed) == 0) {
      getSubStr(sLogSpeed, sUuid03, 24, 23 + UUID_LEN);
      getSubStr(sLogSpeed, sSpeed03, 26 + UUID_LEN, strlen(sLogSpeed) - 2);
      if (strcmp(sSpeed03, END_SPEED_STREAM) == 0)
        iEndSpeed03 = 1;
    } else {
      strcpy(sUuid03, "ERR");
      strcpy(sSpeed03, "ERR PFC 03");
    }
    sprintf(sLogWES, MSG_LOG_PFC03_READING, sUuid03, sSpeed03);
    addLog(LOG_WES, sLogWES);
    /* END   - SPPED PFC 03  */

    if (iEndSpeed01 || iEndSpeed02 || iEndSpeed03)
      break;

    if (strcmp(sSpeed01, sSpeed02) == 0 && strcmp(sSpeed02, sSpeed03) == 0) {
      strcpy(sLogWES, "");
      sprintf(sLogWES, MSG_LOG_STATUS_OK, sUuid, sSpeed01, sSpeed02, sSpeed03);
      printf(MSG_OUTPUT_STATUS_OK, sUuid, sSpeed01, sSpeed02, sSpeed03);
      addLog(STATUS, sLogWES);
    } else {
      if (strcmp(sSpeed01, sSpeed02) == 0 || strcmp(sSpeed02, sSpeed03) == 0 ||
          strcmp(sSpeed01, sSpeed03) == 0) {
        strcpy(sLogWES, "");
        sprintf(sLogWES, MSG_LOG_STATUS_ERROR, sUuid, sSpeed01, sSpeed02,
                sSpeed03);
        printf(MSG_OUTPUT_STATUS_ERROR, sUuid, sSpeed01, sSpeed02, sSpeed03);
        addLog(STATUS, sLogWES);
      } else {
        strcpy(sLogWES, "");
        sprintf(sLogWES, MSG_LOG_STATUS_EMERGENCY, sUuid, sSpeed01, sSpeed02,
                sSpeed03);
        printf(MSG_OUTPUT_STATUS_EMERGENCY, sUuid, sSpeed01, sSpeed02,
               sSpeed03);
        addLog(STATUS, sLogWES);
      }
    }

    mssleep(1050);
  }
}

/*
 * DESCRIPTION
 * ...
 *
 * PARAMETERS
 * ...
 *
 * RETURN VALUES
 * ...
 */
int readSpeed(char *speed_stream, char *speed)
{
  FILE *fp;
  int len = 0;
  int res = 0;
  char c;


  len = 1;
  fp = fopen(speed_stream, "r");
  fseek(fp, -1, SEEK_END);
  c = fgetc(fp);
  while(c == '\n')
    {
      fseek(fp, -2, SEEK_CUR);
      c = fgetc(fp);
      len++;
    }
  while(c != '\n')
    {
      fseek(fp, -2, SEEK_CUR);
      len++;
      c = fgetc(fp);
    }
  fseek(fp, 0, SEEK_CUR);
  if (fgets(speed, len, fp) != NULL) {
    res = 0;
  } else {
    res = 1;
  }
  fclose(fp);
  return res;
}

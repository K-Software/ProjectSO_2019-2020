/* -------------------------------------------------------------------------- */
/* process_util.c                                                             */
/* -------------------------------------------------------------------------- */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "log.h"
#include "process_util.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

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
void setPFC01Pid(int pid)
{
  FILE *fp;
  char sPid[PID_LEN];

  fp = fopen(PATH_PFC01_PID, "w");
  sprintf(sPid,"%d",pid);
  fputs(sPid, fp);
  fclose(fp);
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
void setPFC02Pid(int pid)
{
  FILE *fp;
  char sPid[PID_LEN];

  fp = fopen(PATH_PFC02_PID, "w");
  sprintf(sPid,"%d",pid);
  fputs(sPid, fp);
  fclose(fp);
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
void setPFC03Pid(int pid)
{
  FILE *fp;
  char sPid[PID_LEN];

  fp = fopen(PATH_PFC03_PID, "w");
  sprintf(sPid,"%d",pid);
  fputs(sPid, fp);
  fclose(fp);
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
void getPFC01Pid(char *sPid)
{
  FILE *fp;

  fp = fopen(PATH_PFC01_PID, "r");
  fgets(sPid, PID_LEN, fp);
  fclose(fp);
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
void getPFC02Pid(char *sPid)
{
  FILE *fp;

  fp = fopen(PATH_PFC02_PID, "r");
  fgets(sPid, PID_LEN, fp);
  fclose(fp);
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
void getPFC03Pid(char *sPid)
{
  FILE *fp;

  fp = fopen(PATH_PFC03_PID, "r");
  fgets(sPid, PID_LEN, fp);
  fclose(fp);
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
void setPFC01Status(char *status)
{
  FILE *fp;

  fp = fopen(PATH_PFC01_STATUS, "w");
  fputs(status, fp);
  fclose(fp);
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
void setPFC02Status(char *status)
{
  FILE *fp;

  fp = fopen(PATH_PFC02_STATUS, "w");
  fputs(status, fp);
  fclose(fp);
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
void setPFC03Status(char *status)
{
  FILE *fp;

  fp = fopen(PATH_PFC03_STATUS, "w");
  fputs(status, fp);
  fclose(fp);
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
void getPFC01Status(char *status)
{
  FILE *fp;

  fp = fopen(PATH_PFC01_STATUS, "r");
  if (fp != NULL) {
    fgets(status, PFC_STATUS_LEN, fp);
  } else {
    strcpy(status, PFC_STATUS_RUN);
  }
  fclose(fp);
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
void getPFC02Status(char *status)
{
  FILE *fp;

  fp = fopen(PATH_PFC02_STATUS, "r");
  if (fp != NULL) {
    fgets(status, PFC_STATUS_LEN, fp);
  } else {
    strcpy(status, PFC_STATUS_RUN);
  }
  fclose(fp);
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
void getPFC03Status(char *status)
{
  FILE *fp;

  fp = fopen(PATH_PFC03_STATUS, "r");
  if (fp != NULL) {
    fgets(status, PFC_STATUS_LEN, fp);
  } else {
    strcpy(status, PFC_STATUS_RUN);
  }
  fclose(fp);
}

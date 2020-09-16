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
 * This function writes in a specific temporary file the pid of the 
 * PFC 01 process.
 *
 * PARAMETERS
 * - pid = pid of the process
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
 * This function writes in a specific temporary file the pid of the 
 * PFC 02 process.
 *
 * PARAMETERS
 * - pid = pid of the process
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
 * This function writes in a specific temporary file the pid of the 
 * PFC 03 process.
 *
 * PARAMETERS
 * - pid = pid of the process
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
 * This function writes in a specific temporary file the pid of the 
 * Reader process.
 *
 * PARAMETERS
 * - pid = pid of the process
 */
void setReaderPid(int pid)
{
  FILE *fp;
  char sPid[PID_LEN];

  fp = fopen(PATH_READER_PID, "w");
  sprintf(sPid,"%d",pid);
  fputs(sPid, fp);
  fclose(fp);
}

/*
 * DESCRIPTION
 * This function writes in a specific temporary file the pid of the 
 * Transducers process.
 *
 * PARAMETERS
 * - pid = pid of the process
 */
void setTransPid(int pid)
{
  FILE *fp;
  char sPid[PID_LEN];

  fp = fopen(PATH_TRANS_PID, "w");
  sprintf(sPid,"%d",pid);
  fputs(sPid, fp);
  fclose(fp);
}

/*
 * DESCRIPTION
 * This function writes in a specific temporary file the pid of the 
 * Failure generator process.
 *
 * PARAMETERS
 * - pid = pid of the process
 */
void setFailGenPid(int pid)
{
  FILE *fp;
  char sPid[PID_LEN];

  fp = fopen(PATH_FAILG_PID, "w");
  sprintf(sPid,"%d",pid);
  fputs(sPid, fp);
  fclose(fp);
}

/*
 * DESCRIPTION
 * This function reads from a specific temporary file the pid of the 
 * PFC 01 process.
 *
 * PARAMETERS
 * - pid = pid of the process
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
 * This function reads from a specific temporary file the pid of the 
 * PFC 02 process.
 *
 * PARAMETERS
 * - pid = pid of the process
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
 * This function reads from a specific temporary file the pid of the 
 * PFC 03 process.
 *
 * PARAMETERS
 * - pid = pid of the process
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
 * This function reads from a specific temporary file the pid of the 
 * Reader process.
 *
 * PARAMETERS
 * - pid = pid of the process
 */
void getReaderPid(char *sPid)
{
  FILE *fp;

  fp = fopen(PATH_READER_PID, "r");
  fgets(sPid, PID_LEN, fp);
  fclose(fp);
}

/*
 * DESCRIPTION
 * This function reads from a specific temporary file the pid of the 
 * Transducers process.
 *
 * PARAMETERS
 * - pid = pid of the process
 */
void getTransPid(char *sPid)
{
  FILE *fp;

  fp = fopen(PATH_TRANS_PID, "r");
  fgets(sPid, PID_LEN, fp);
  fclose(fp);
}

/*
 * DESCRIPTION
 * This function reads from a specific temporary file the pid of the 
 * Failure Generator process.
 *
 * PARAMETERS
 * - pid = pid of the process
 */
void getFailGenPid(char *sPid)
{
  FILE *fp;

  fp = fopen(PATH_FAILG_PID, "r");
  fgets(sPid, PID_LEN, fp);
  fclose(fp);
}
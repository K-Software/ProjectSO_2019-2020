/* -------------------------------------------------------------------------- */
/* failure_generator.c                                                        */
/* -------------------------------------------------------------------------- */
#include <bsd/stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "common.h"
#include "failure_generator.h"
#include "log.h"
#include "pfcs_util.h"
#include "process_util.h"
#include "time_util.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define LOG_FAILURE "FAILURE"
#define PFC_01_NAME "PFC 01"
#define PFC_02_NAME "PFC 02"
#define PFC_03_NAME "PFC 03"
#define MSG_LOG_FAILURE_SIGINT "FAILURE - Type: SIGINT - Process: %s - PID: %s"
#define MSG_LOG_FAILURE_SIGSTOP "FAILURE - Type: SIGSTOP - Process: %s - PID: %s"
#define MSG_LOG_FAILURE_SIGCONT "FAILURE - Type: SIGCONT - Process: %s - PID: %s"
#define MSG_LOG_FAILURE_SIGUSR1 "FAILURE - Type: SIGUSR1 - Process: %s - PID: %s"
#define MSG_LOG_FAILURE_PROB "FAILURE - SIGINT: %d - SIGSTOP : %d - SIGCONT: %d - SIGUSR1: %d"

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

/*
 * DESCRIPTION
 * This function creates the Failure Generator procress.
 */
void createFailureGenerator(void)
{
  char sLogMsg[MAX_ROW_LEN_LOG] = "";
  char sPid[PID_LEN] = "";
  int counter = 0;
  char gpgll[GPGLL_LEN] = "";
  char coord[MAX_ROW_LEN_G18] = "";

  setFailGenPid(getpid());
  sprintf(sLogMsg,  MSG_LOG_FAILURE_PROB, FAILURE_SIGINT,
          FAILURE_SIGSTOP, FAILURE_SIGCONT, FAILURE_SIGUSR1);
  addLog(LOG_FAILURE, sLogMsg);
  while (1) {
    switch(selectPFC()) {
    case 1: /* PFC 01 */
      if (access(PATH_PFC01_PID, F_OK) != -1) {
        strcpy(sPid, "");
        getPFC01Pid(sPid);
        failurePFC(PFC_01_NAME, sPid);
      }
      break;

    case 2: /* PFC 02 */
      if (access(PATH_PFC02_PID, F_OK) != -1) {
        strcpy(sPid, "");
        getPFC02Pid(sPid);
        failurePFC(PFC_02_NAME, sPid);
      }
      break;

    case 3: /* PFC 03 */
      if (access(PATH_PFC03_PID, F_OK) != -1) {
        strcpy(sPid, "");
        getPFC03Pid(sPid);
        failurePFC(PFC_03_NAME, sPid);
      }
    }

    readCoord(coord);
    getGPGLL(gpgll, coord);
    if (strcmp("END", gpgll) == 0)
      break;

    mssleep(1000);
  }
}

/*
 * DESCRIPTION
 * This function select a random number between 1 to 3 for select one of PFC.
 *
 * RETURN VALUES
 * Return an integer number between 1 to 3.
 */
int selectPFC(void)
{
  unsigned int seed;
  int r = 0;

  r = (arc4random_uniform(3)) + 1;
  return r;
}

/*
 * DESCRIPTION
 * This function determines if it occurred a SIGINT signal.
 *
 * RETURN PARAMETERS
 * Return 1 if it occurred the SIGINT signal, Otherwise, return 0.
 */
int failureSIGINT(void)
{
  int r;

  r = arc4random_uniform(FAILURE_SIGINT);
  return r == 0;
}

/*
 * DESCRIPTION
 * This function determines if it occurred a SIGSTOP signal.
 *
 * RETURN VALUES
 * Return 1 if it occurred the SIGSTOP signal, Otherwise, return 0.
 */
int failureSIGSTOP(void)
{
  int r;

  r = arc4random_uniform(FAILURE_SIGSTOP);
  return r == 0;
}

/*
 * DESCRIPTION
 * This function determines if it occurred a SIGCONT signal.
 *
 * RETURN PARAMETERS
 * Return 1 if it occurred the SIGCONT signal, Otherwise, return 0.
 */
int failureSIGCONT(void)
{
  int r;

  r = arc4random_uniform(FAILURE_SIGCONT);
  return r == 0;
}

/*
 * DESCRIPTION
 * This function determines if it occurred a SIGUSR1 signal.
 *
 * RETURN PARAMETERS
 * Return 1 if it occurred the SIGUSR1 signal, Otherwise, return 0.
 */
int failureSIGUSR1(void)
{
  int r;

  r = arc4random_uniform(FAILURE_SIGUSR1);
  return r == 0;
}

/*
 * DESCRIPTION
 * This function tries to send the signals SIGINT, SIGSTOP, SIGCONT, and 
 * SIGURS1 to process identified by the parameter pid.
 *
 * PARAMETERS
 * - namePfc = name of PFC(PFC 01, PFC 02, PFC03)
 * - pid = pid of PFC process
 *
 */
void failurePFC(char *namePfc, char *pid)
{
  char sFailureMsg[MAX_ROW_LEN_LOG] = "";
  int iPid;
  int iStatus;

  iPid = atoi(pid);
  if (failureSIGINT()) {
    kill(iPid, SIGINT);
    strcpy(sFailureMsg, "");
    sprintf(sFailureMsg, MSG_LOG_FAILURE_SIGINT, namePfc, pid);
    addLog(LOG_FAILURE, sFailureMsg);
  }

  if (failureSIGSTOP()) {
    kill(iPid, SIGSTOP);
    strcpy(sFailureMsg, "");
    sprintf(sFailureMsg, MSG_LOG_FAILURE_SIGSTOP, namePfc, pid);
    addLog(LOG_FAILURE, sFailureMsg);
  }

  if (failureSIGCONT()) {
    kill(iPid, SIGCONT);
    strcpy(sFailureMsg, "");
    sprintf(sFailureMsg, MSG_LOG_FAILURE_SIGCONT, namePfc, pid);
    addLog(LOG_FAILURE, sFailureMsg);
  }

  if (failureSIGUSR1()) {
    kill(iPid, SIGUSR1);
    strcpy(sFailureMsg, "");
    sprintf(sFailureMsg, MSG_LOG_FAILURE_SIGUSR1, namePfc, pid);
    addLog(LOG_FAILURE, sFailureMsg);
  }
}
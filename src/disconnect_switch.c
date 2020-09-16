/* -------------------------------------------------------------------------- */
/* disconnect_switch.c                                                        */
/* -------------------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/errno.h>
#include "common.h"
#include "disconnect_switch.h"
#include "error_channel.h"
#include "log.h"
#include "process_util.h"
#include "signal.h"
#include "string_util.h"
#include "time_util.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define LOG_DISC_SWITCH "DISC_SWITCH"
#define MSG_LOG_START "createDiscSwitch - START"
#define MSG_LOG_CHECK_PROC_STATUS "CHECK STATUS OF PROCESS NUM: %s (PFC_%s) - STATUS: %s %s"
#define MSG_LOG_START_PROC "RESTART (SIGCONT) PROCESS NUM: %s (PFC_%s)"
#define MSG_LOG_END "createDiscSwitch - END"

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

/*
 * DESCRIPTION
 * This function creates the PFC Disconnect Switch process
 */
void createDiscSwitch(void)
{
  FILE *fp;
  int ppErr;                                                  /* Pipe pointer */
  char sErr[ERROR_VALUE_LEN + 1] = "";                        /* Error        */
  char sPfcNum[2] = "";
  char sPid[PID_LEN] = "";
  char sPFC01Pid[PID_LEN] = "";
  char sPFC02Pid[PID_LEN] = "";
  char sPFC03Pid[PID_LEN] = "";
  char sReaderPid[PID_LEN] = "";
  char sTransPid[PID_LEN] = "";
  char sFailGenPid[PID_LEN] = "";
  int iPid = 0;
  char sStatus[30] = "";
  char sPath[50] = "";
  char sLogMsg[MAX_ROW_LEN_LOG] = "";
  char sLabName[6] = "";
  char sNamePro[100] = "";
  char sLabState[7] = "";
  char sStateCode[2] = "";
  char sStateDesc[20] = "";

  openErrChnnlRead(&ppErr);                /* Open pipe channel in read mode  */

  addLog(LOG_DISC_SWITCH, MSG_LOG_START);
  while (1) {
    readErrChnnl(ppErr, sErr);
    addLog(LOG_DISC_SWITCH, sErr);
    if (startWith(sErr, "ERROR")) {
      getSubStr(sErr, sPfcNum, 6, 7);
      if (strcmp(sPfcNum, "01") == 0) {
        getPFC01Pid(sPid);
      }
      if (strcmp(sPfcNum, "02") == 0) {
        getPFC02Pid(sPid);
      }
      if (strcmp(sPfcNum, "03") == 0) {
        getPFC03Pid(sPid);
      }
      iPid = atoi(sPid);
      if (kill(iPid, 0) == 0) {
        sprintf(sPath, "/proc/%d/status", iPid);
        fp = fopen(sPath, "r");
        fscanf(fp, "%s %s %s %s %s", sLabName, sNamePro, sLabState,
               sStateCode , sStateDesc);             /* Get status of process */
        fclose(fp);
        sprintf(sLogMsg,
                MSG_LOG_CHECK_PROC_STATUS,
                sPid, sPfcNum, sStateCode, sStateDesc);  /* Status of process */
        addLog(SWITCH, sLogMsg);
        if (strcmp("T", sStateCode) == 0) {
          kill(iPid, SIGCONT);                           /* Restart process   */
          sprintf(sLogMsg, MSG_LOG_START_PROC, sPid, sPfcNum);
          addLog(SWITCH, sLogMsg);
        }
      } else {
        sprintf(sLogMsg,
                "CHECK STATUS OF PROCESS NUM: %s (PFC_%s) - STATUS: don't exist",
                sPid, sPfcNum);
        addLog(SWITCH, sLogMsg);
      }
    }
    if (startWith(sErr, "EMERG")) {
      getPFC01Pid(sPFC01Pid);
      getPFC02Pid(sPFC02Pid);
      getPFC03Pid(sPFC03Pid);
      getReaderPid(sReaderPid);
      getTransPid(sTransPid);
      getFailGenPid(sFailGenPid);
      kill(atoi(sPFC01Pid), SIGKILL);
      kill(atoi(sPFC02Pid), SIGKILL);
      kill(atoi(sPFC03Pid), SIGKILL);
      kill(atoi(sReaderPid), SIGKILL);
      kill(atoi(sTransPid), SIGKILL);
      kill(atoi(sFailGenPid), SIGKILL);
      addLog(SWITCH, "TERMINATE PROJECT");
      break;
    }
    mssleep(1000);
  }

  closeErrChnnlRead(ppErr);                /* Close pipe channel in read mode */
  addLog(LOG_DISC_SWITCH, MSG_LOG_END);
}

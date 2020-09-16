/* -------------------------------------------------------------------------- */
/* project.c                                                                  */
/* -------------------------------------------------------------------------- */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <unistd.h>
#include "common.h"
#include "failure_generator.h"
#include "disconnect_switch.h"
#include "log.h"
#include "pfcs.h"
#include "pfcs_util.h"
#include "project.h"
#include "signal.h"
#include "time_util.h"
#include "transducers.h"
#include "wes.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define LOG_PROJECT "PROJECT_SO"
#define MSG_PROJECT_START "project - start"
#define MSG_PROJECT_START_DISC_SWITCH "project - PFC Disconnect Switch start"
#define MSG_PROJECT_END_DISC_SWITCH "project - PFC Disconnect Switch end"
#define MSG_PROJECT_START_WES "project - WES start"
#define MSG_PROJECT_END_WES "project - WES end"
#define MSG_PROJECT_START_TRANS "project - Transducers start"
#define MSG_PROJECT_END_TRANS "project - Transducers end"
#define MSG_PROJECT_START_PFCS "project - PFCS start"
#define MSG_PROJECT_END_PFCS "project - PFCS end"
#define MSG_PROJECT_START_FAILGEN "project - Failure Generator start"
#define MSG_PROJECT_END_FAILGEN "project - Failure Generator end"
#define MSG_PROJECT_END "project - end"

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
  int pidDiscSwitch = 0;
  int pidWES = 0;
  int pidTran = 0;
  int pidPFCS = 0;
  int pidFailGen = 0;

  if (argc == 2) {

    projectInit();

    addLog(LOG_PROJECT, MSG_PROJECT_START);
    pidDiscSwitch = fork();
    if (pidDiscSwitch == 0) {

      addLog(LOG_PROJECT, MSG_PROJECT_START_DISC_SWITCH);
      createDiscSwitch();                                /* Disconnect Switch */
      addLog(LOG_PROJECT, MSG_PROJECT_END_DISC_SWITCH);
    } else {
      pidWES = fork();
      if (pidWES == 0) {

        addLog(LOG_PROJECT, MSG_PROJECT_START_WES);
        createWES();                                     /* WES               */
        addLog(LOG_PROJECT, MSG_PROJECT_END_WES);
      } else {
        pidTran = fork();
        if (pidTran == 0) {

          addLog(LOG_PROJECT, MSG_PROJECT_START_TRANS);
          createTranducers();                            /* Transducers       */
          addLog(LOG_PROJECT, MSG_PROJECT_END_TRANS);
        } else {
          pidPFCS = fork();
          if (pidPFCS == 0) {

            addLog(LOG_PROJECT, MSG_PROJECT_START_PFCS);
            createPFCS(argv[1]);                         /* PFCS              */
            addLog(LOG_PROJECT, MSG_PROJECT_END_PFCS);
          } else {
            pidFailGen = fork();
            if (pidFailGen == 0) {

              addLog(LOG_PROJECT, MSG_PROJECT_START_FAILGEN);
              createFailureGenerator();                  /* Failure Generator */
              addLog(LOG_PROJECT, MSG_PROJECT_END_FAILGEN);
            } else {

              // Project
              for (int i=0; i< 4; i++) {
                wait(NULL);
              }
              kill(pidDiscSwitch, SIGINT);
              addLog(LOG_PROJECT, MSG_PROJECT_END_DISC_SWITCH);
              addLog(LOG_PROJECT, MSG_PROJECT_END);
            }
          }
        }
      }
    }
  } else {
    printf("Wrong numbers of argoments\n");
  }
}

/*
 * DESCRIPTION
 * This function removes the temporary files used for communication between
 * transduces and the PFCS.
 */
void projectInit(void)
{
  char msgLog[MAX_ROW_LEN_LOG];

  if (unlink(PATH_FILE_CHANNEL) == 0) {              /* Remove temporary file */
    addLog(LOG_PROJECT, "Remove file: OK");
  } else {
    sprintf(msgLog, "Remove file: ERROR - %s", strerror(errno));
    addLog(LOG_PROJECT, msgLog);                     /* Occur an error during */
                                                     /* removing the          */
                                                     /* temporary file        */
  }

  if (unlink(PATH_PIPE_CHANNEL) == 0) {              /* Remove temporary pipe */
    addLog(LOG_PROJECT, "Remove pipe: OK");
  } else {
    strcpy(msgLog, "");
    sprintf(msgLog, "Remove pipe: ERROR - %s", strerror(errno));
    addLog(LOG_PROJECT, msgLog);                     /* Occur an error during */
                                                     /* removing the          */
                                                     /* temporary pipe        */
  }

  if (mkfifo(PATH_PIPE_CHANNEL, 0666) == 0) {        /* Create temporary pipe */
    addLog(LOG_PROJECT, "Create pipe: OK");
  } else {
    strcpy(msgLog, "");
    sprintf(msgLog, "create pipe: ERROR - %s", strerror(errno));
    addLog(LOG_PROJECT, msgLog);                     /* Occur error during    */
                                                     /* creation of temporary */
                                                     /* pipe */
  }

  if (unlink(PATH_ERROR_CHANNEL) == 0) {             /* Remove temporary pipe */
    addLog(LOG_PROJECT, "Remove pipe error: OK");
  } else {
    strcpy(msgLog, "");
    sprintf(msgLog, "Remove pipe error: ERROR - %s", strerror(errno));
    addLog(LOG_PROJECT, msgLog);                     /* Occur an error during */
                                                     /* removing the          */
                                                     /* temporary pipe        */
  }

  if (mkfifo(PATH_ERROR_CHANNEL, 0666) == 0) {       /* Create temporary pipe */
    addLog(LOG_PROJECT, "Create pipe error: OK");
  } else {
    strcpy(msgLog, "");
    sprintf(msgLog, "create pipe error: ERROR - %s", strerror(errno));
    addLog(LOG_PROJECT, msgLog);                     /* Occur error during    */
                                                     /* creation of temporary */
                                                     /* pipe */
  }

  /* Remove all files with velocities created from the previous execution     */
  /* of project.                                                              */
  removeLog(SPEED_PFC_01);
  removeLog(SPEED_PFC_02);
  removeLog(SPEED_PFC_03);

  removeLog(STATUS);                       /* Remove previous STATUS log file */

  removeCoord();
}

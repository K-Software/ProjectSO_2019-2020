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
#include <unistd.h>
#include "common.h"
#include "log.h"
#include "pfcs.h"
#include "project.h"
#include "time_util.h"
#include "transducers.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define LOG_PROJECT "PROJECT_SO"

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

int main(int argc, char *argv[])
{
  int pidTran;
  int pidPFCS;

  if (argc == 2) {

    projectInit();

    addLog(LOG_PROJECT, "Start - Project");
    pidTran = fork();
    if (pidTran == 0) {

      // Transducers
      createTranducers();
    } else {
        pidPFCS = fork();
        if (pidPFCS == 0) {

        // PFCS
        createPFCS(argv[1]);
      } else {

        // Project
        for (int i=0; i< 2; i++) {
          wait(NULL);
        }
        addLog(LOG_PROJECT, "Finish - Project");
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

  if (mkfifo(PATH_PIPE_CHANNEL, 0600) == 0) {        /* Create temporary pipe */
    addLog(LOG_PROJECT, "Create pipe: OK");
  } else {
    strcpy(msgLog, "");
    sprintf(msgLog, "create pipe: ERROR - %s", strerror(errno));
    addLog(LOG_PROJECT, msgLog);                     /* Occur error during    */
                                                     /* creation of temporary */
                                                     /* pipe */
  }

  /*
   * Remove all files with velocities created from the previous execution
   * of project.
   */
  removeLog(SPEED_PFC_01);
  removeLog(SPEED_PFC_02);
  removeLog(SPEED_PFC_03);
}

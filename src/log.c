/* -------------------------------------------------------------------------- */
/* log.c                                                                      */
/* -------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include "common.h"
#include "log.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define PATH_LOG "./log/"
#define EXT_LOG ".log"
#define PREFIX_LEN 25

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

#ifdef DEBUG
int main(int argc, char *argv[])
{
  if (argc > 1) {
    addLog(argv[1], argv[2]);
  } else {
    printf("Wrong number of parameters\n");
  }
}
#endif

/*
 * DESCRIPTION
 * This function adds the value of the parameter msg into the log file identify
 * by parameter id.
 *
 * PARAMETERS:
 * - id = identifier of the log file;
 * - msg = message to add.
 *
 * RETURN VALUES:
 * - 0 = Message added into the log file;
 * - 1 = Message too long;
 * - 2 = Occur error during opening file;
 * - 3 = Occur error during writing file.
 */
int addLog(char *id, char *msg)
{
  FILE *fpLog;
  char *fileName = buildFileName(id);

  /* Add control on length of message */
  if (strlen(msg) > MAX_ROW_LEN_LOG) {
    return 1;                                             /* Message too long */
  }

  fpLog = fopen(fileName, "a");           /* Open the log file in append mode */
  if (fpLog == NULL) {
    free(fileName);
    return 2;                    /* Occur an error during the opening of file */
  }

  char *tmpMsg = enrichMsg(msg);                        /* Enrich the message */

  /* Write log message */
  if (fputs(tmpMsg, fpLog) == EOF ) {
    free(tmpMsg);
    free(fileName);
    fclose(fpLog);
    return 3;                    /* Occur an error during the writing of file */
  };

  free(tmpMsg);
  free(fileName);
  fclose(fpLog);
  return 0;
}

/*
 * DESCRIPTION
 * This function removes a specific log file.
 *
 * PARAMETERS
 * - id = Identifier of the log file
 *
 * RETURN VALUES
 * Upon successful completion, removeLog returns 0. Otherwise, return a value
 * different than 0.
 * If return 1 there was an error during the removing of log file.
 */
int removeLog(char *id)
{
  char *fileName = buildFileName(id);

  if (unlink(fileName) == -1) {                            /* Remove log file */
    return 1;                  /* Occur an error during  the removing of file */
  };
  return 0;
}


/*
 * DESCRIPTION
 * This function builds the name of the log file used param id and macros
 * PATH_LOG and EXT_LOG. Complete the name of the file added the date of
 * creation.
 * Example:
 * ./log/test_20200620.log
 *
 * PARAMETERS
 * - id = Identifier of the log file
 *
 * RETURN VALUES:
 * Return the relative path of file log.
 */
char* buildFileName(char *id)
{
  time_t now;
  char *fileName = malloc(strlen(PATH_LOG)+strlen(id)+strlen(EXT_LOG)+10);
  time(&now);

  char sToday[9];
  strftime(sToday, sizeof(sToday), "%Y%m%d", localtime(&now));
  strcpy(fileName,PATH_LOG);
  strcat(fileName,id);
  strcat(fileName,"_");
  strcat(fileName,sToday);
  strcat(fileName,EXT_LOG);
  return fileName;
}

/*
 * DESCRIPTION
 * This function enriches the message with the specific info useful for the log.
 *
 * PARAMETERS:
 * - msg = text of massege to be enriched.
 *
 * RETURN PARAMETERS:
 * Return the text of message added with the prefix '[yyyymmdd-hh:MM:ss:mmm]:'
 * and postfix '\n'.
 */
char* enrichMsg(char *msg)
{
  struct timeval tp;
  gettimeofday(&tp, 0);
  time_t now = tp.tv_sec;
  char *tmpMsg = malloc(PREFIX_LEN + strlen(msg) + 2);
  struct tm *t = localtime(&now);
  char tt[7];
  sprintf(tt, ":%03d]:", tp.tv_usec/1000);
  char prefix[PREFIX_LEN];
  strftime(prefix, sizeof(prefix), "[%Y%m%d-%X", localtime(&now));
  strcat(prefix, tt);
  strcpy(tmpMsg, prefix);
  strcat(tmpMsg, msg);
  strcat(tmpMsg, "\n");
  return tmpMsg;
}

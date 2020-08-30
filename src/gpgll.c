/* -------------------------------------------------------------------------- */
/* gpgll.c                                                                    */
/* -------------------------------------------------------------------------- */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "common.h"
#include "gpgll.h"
#include "log.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define PATH_GPGLL "./data/GPGLL.txt"
#define PATH_GPGLL_TMP "./tmp/GPGLL.tmp"
#define LOG_GPGLL "GPGLL"
#define MSG_INIT_OPEN_FILE_G18 "init - Open  file: G18.txt"
#define MSG_INIT_CLOSE_FILE_G18 "init - Close file: G18.txt"
#define MSG_INIT_ERR_OPEN_FILE_G18 "init - Error opening file G18"
#define MSG_INIT_OPEN_FILE_GPGLL "init - Open  file: GPGLL.txt"
#define MSG_INIT_CLOSE_FILE_GPGLL "init - Close file: GPGLL.txt"
#define MSG_INIT_ERR_OPEN_FILE_GPGLL "init - Error opening file GPGLL"
#define MSG_INIT_GPGLL_ADD_ROWS "init - Added rows: "
#define MSG_POP_OPEN_FILE_GPGLL "pop  - Open  file: GPGLL.txt"
#define MSG_POP_CLOSE_FILE_GPGLL "pop  - Close file: GPGLL.txt"
#define MSG_POP_OPEN_FILE_TMP "pop  - Open  file: temporary file"
#define MSG_POP_CLOSE_FILE_TMP "pop  - Close file: temporary file"
#define MSG_POP_ROW "pop  - Pop row: "
#define MSG_POP_EOF "pop  - EOF"
#define MSG_POP_ERR_OPEN_FILE_GPGLL "pop  - Error opening file GPGLL"
#define MSG_POP_ERR_DEL_FILE_GPGLL "pop  - Error removing file GPGLL"
#define MSG_POP_ERR_OPEN_FILE_TMP "pop  - Error opening temporary file"
#define MSG_POP_ERR_DEL_FILE_TMP "pop  - Error removing temporary file"
#define MSG_POP_ERR_MV_FILE_TMP "pop  - Error renaming temporary file"

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

#ifdef DEBUG
int main(int argc, char *argv[])
{
  char row[MAX_ROW_LEN_G18];
  GPGLLInit(argv[1]);
  printf("pop: %s\n", GPGLLPop(row));
  printf("pop: %s\n", GPGLLPop(row));
  printf("pop: %s\n", GPGLLPop(row));
}
#endif

/*
 * DESCRIPTION
 * This function produces the file GPGLL.txt from file G18.txt.
 * The file GPGLL.txt contains only rows of file G18.txt that start with
 * "$GPGLL".
 *
 * PARAMETERS:
 * - path = path of file G18.txt
 *
 * RETURN VALUES:
 * - 0 = all right;
 * - 1 = Error opening file G18;
 * - 2 = Error opening file GPGLL;
 */
int GPGLLInit (char *path)
{
  FILE *fpG18;
  FILE *fpGPGLL;
  char row[MAX_ROW_LEN_G18];

  fpG18 = fopen(path, "r");         /* Open the file G18.txt in read mode */
  if (fpG18 == NULL) {
    addLog(LOG_GPGLL, MSG_INIT_ERR_OPEN_FILE_G18);
    return 1;
  }
  addLog(LOG_GPGLL, MSG_INIT_OPEN_FILE_G18);

  fpGPGLL = fopen(PATH_GPGLL, "w"); /* Open the file GPGLL.txt in write mode */
  if (fpGPGLL == NULL) {
    addLog(LOG_GPGLL, MSG_INIT_ERR_OPEN_FILE_GPGLL);
    return 2;
  }
  addLog(LOG_GPGLL, MSG_INIT_OPEN_FILE_GPGLL);

  /* If row start with $GPGLL puts the row inside the file GPGLL.txt */
  int iRows = 0;
  while (fgets(row, MAX_ROW_LEN_G18, fpG18) != NULL) {
    if (strncmp(row, "$GPGLL", 6) == 0) {
      fputs(row, fpGPGLL);
      iRows++;
    }
  }
  char sRows[12];
  sprintf(sRows, "%11d", iRows);
  char *tmpMsg = malloc(strlen(MSG_INIT_GPGLL_ADD_ROWS) + strlen(sRows));
  strcpy(tmpMsg, MSG_INIT_GPGLL_ADD_ROWS);
  strcat(tmpMsg, sRows);
  addLog(LOG_GPGLL, tmpMsg);
  free(tmpMsg);

  fclose(fpGPGLL); /* Close the file fbGPGLL.txt */
  addLog(LOG_GPGLL, MSG_INIT_CLOSE_FILE_GPGLL);
  fclose(fpG18);   /* Close the file fbG18.tx */
  addLog(LOG_GPGLL, MSG_INIT_CLOSE_FILE_G18);
  return 0;
}

/*
 * DESCRIPTION
 * This function remove the oldest row from the file GPGLL.txt like the pop
 * function of FIFO data structure.
 *
 * RETURN VALUES
 * Upon successful completion return the oldest row of file GPGLL.txt.
 * Otherwise, NULL is returned.
 */
char* GPGLLPop(char *row)
{
  FILE *fpGPGLL;
  FILE *fpTmp;
  int status;
  char rowTmp[MAX_ROW_LEN_G18];

  fpGPGLL = fopen(PATH_GPGLL, "r"); /* Open the file GPGLL in read mode */
  if (fpGPGLL == NULL) {
    addLog(LOG_GPGLL, MSG_POP_ERR_OPEN_FILE_GPGLL);
    return NULL;
  }
  addLog(LOG_GPGLL, MSG_POP_OPEN_FILE_GPGLL);

  /* Check if exist the temporary file */
  if (access(PATH_GPGLL_TMP, F_OK) != -1) {
    if (remove(PATH_GPGLL_TMP) != 0) {
      addLog(LOG_GPGLL, MSG_POP_ERR_DEL_FILE_TMP);
      return NULL;
    }
  }

  fpTmp = fopen(PATH_GPGLL_TMP, "a"); /* Open a temporary file in append mode */
  if (fpTmp == NULL) {
    addLog(LOG_GPGLL, MSG_POP_ERR_OPEN_FILE_TMP);
    return NULL;
  }
  addLog(LOG_GPGLL, MSG_POP_OPEN_FILE_TMP);

  fgets(row, MAX_ROW_LEN_G18, fpGPGLL);
  if (feof(fpGPGLL)) {
    addLog(LOG_GPGLL, MSG_POP_EOF);

    fclose(fpTmp);   /* Close the file fpTmp */
    addLog(LOG_GPGLL, MSG_POP_CLOSE_FILE_TMP);
    fclose(fpGPGLL); /* Close the file fpGPGLL */
    addLog(LOG_GPGLL, MSG_POP_CLOSE_FILE_GPGLL);
    return NULL;
  } else {
    char *msgTmp = malloc(strlen(MSG_POP_ROW) + strlen(row));
    strcpy(msgTmp, MSG_POP_ROW);
    strcat(msgTmp, row);
    addLog(LOG_GPGLL, msgTmp);
    free(msgTmp);

    while (fgets(rowTmp, MAX_ROW_LEN_G18, fpGPGLL) != NULL) {
      fputs(rowTmp, fpTmp);
    }

    fclose(fpTmp);   /* Close the file fpTmp */
    addLog(LOG_GPGLL, MSG_POP_CLOSE_FILE_TMP);
    fclose(fpGPGLL); /* Close the file fpGPGLL */
    addLog(LOG_GPGLL, MSG_POP_CLOSE_FILE_GPGLL);

    /* Remove the old GPGLL file */
    if (remove(PATH_GPGLL) != 0) {
      addLog(LOG_GPGLL, MSG_POP_ERR_DEL_FILE_GPGLL);
      return NULL;
    }

    /* Rename tempory file */
    if (rename(PATH_GPGLL_TMP, PATH_GPGLL) != 0) {
      addLog(LOG_GPGLL, MSG_POP_ERR_MV_FILE_TMP);
      return NULL;
    }
    return row;
  }
}

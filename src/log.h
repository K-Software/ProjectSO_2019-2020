/* -------------------------------------------------------------------------- */
/* log.h                                                                      */
/* -------------------------------------------------------------------------- */
int addLog(char *id, char *msg);
int removeLog(char *id);
char* buildFileName(char *id);
char* enrichMsg(char *msg);

/* -------------------------------------------------------------------------- */
/* pfcs_util.h                                                                */
/* -------------------------------------------------------------------------- */
char* buildCreatePFCSStartMsg(char *msg, char *text);
char* buildCreatePFCSFinishMsg(char *msg, char *text);
char* buildReadingMsg(char *msg, char *text);
int writeCoord(char *reading);
char* readCoord(char *coord);
char* getUUID(char *uuid, char *coord);
char* getGPGLL(char *gpgll, char *coord);
double getLat(char *gpgll);
double getLon(char *gpgll);
int getSubStr(char *source, char *target,int from, int to);
int mssleep(long msec);

/* -------------------------------------------------------------------------- */
/* pfcs_util.h                                                                */
/* -------------------------------------------------------------------------- */
char* buildCreatePFCSStartMsg(char *msg, char *text);
char* buildCreatePFCSFinishMsg(char *msg, char *text);
int writeCoord(char *reading);
char* readCoord(char *coord);
int removeCoord(void);
char* getGPGLL(char *gpgll, char *coord);
double getLat(char *gpgll);
double getLon(char *gpgll);

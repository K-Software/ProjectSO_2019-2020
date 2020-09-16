/* -------------------------------------------------------------------------- */
/* string_util.h                                                              */
/* -------------------------------------------------------------------------- */
char* generateUuid(char *uuid);
char* buildLogMsg(char *msg, char *sPrefix, char *sBody);
char* buildUuidMsg(char *msg, char *text);
int getSubStr(char *source, char *target,int from, int to);
char* getUUID(char *uuid, char *string);
int startWith(char *source, char *prefix);

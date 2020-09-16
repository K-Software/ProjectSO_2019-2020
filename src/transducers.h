/* -------------------------------------------------------------------------- */
/* transducers.h                                                              */
/* -------------------------------------------------------------------------- */
void createTranducers(void);
int readFromPipe(int pipePointer, char *uuid);
int readFromSocket(char *uuid);
int readFromFile(char *previsUuid, char *uuid);

/* -------------------------------------------------------------------------- */
/* error_channel.h                                                            */
/* -------------------------------------------------------------------------- */
int openErrChnnlRead(int *pipePointer);
int readErrChnnl(int pipePointer, char *sErr);
int writeErrChnnl(char *sErr);
int closeErrChnnlRead(int pipePointer);

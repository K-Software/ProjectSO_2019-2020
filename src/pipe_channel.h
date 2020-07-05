/* -------------------------------------------------------------------------- */
/* pipe_channel.h                                                             */
/* -------------------------------------------------------------------------- */
int openPipeChnnlRead(int *pipePointer);
int readPipeChnnl(int pipePointer, char *sVel);
int writePipeChnnl(double vel);
int closePipeChnnlWrite(void);
int closePipeChnnlRead(int pipePointer);

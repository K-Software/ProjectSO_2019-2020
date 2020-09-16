/* -------------------------------------------------------------------------- */
/* failure_generator.h                                                        */
/* -------------------------------------------------------------------------- */
void createFailureGenerator(void);
int selectPFC(void);
int failureSIGINT(void);
int failureSIGSTOP(void);
int failureSIGCONT(void);
int failureSIGUSR1(void);
void failurePFC(char *namePFC, char *pid);
void signalPFC(char *namePfc, char *signal);

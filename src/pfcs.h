/* -------------------------------------------------------------------------- */
/* pfcs.h                                                                     */
/* -------------------------------------------------------------------------- */
void pfcsInit(void);
void createPFCS(char *path);
void pfc01(void);
void pfc02(void);
void pfc03(void);
void reader(char *path);
int addLogPFC(int pfcNum, char *msg);
void shifter(int sig);

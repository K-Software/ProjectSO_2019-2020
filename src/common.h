/* -------------------------------------------------------------------------- */
/* common.h                                                                   */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
#define GPGLL_LEN 45                                 /* Length of GPGLL       */
#define MAX_ROW_LEN_G18 100                          /* Max length of G18 row */
#define MAX_ROW_LEN_LOG 300                          /* Max length of log row */
#define PREFIX_LEN_LOG 25                            /* Length of log-prefix  */
#define UUID_LEN 36                                  /* Length of UUID        */
#define NUM_PFCS 4                                   /* Numbers of PFCS       */
#define PID_LEN 10                                   /* Lenght of PID         */
#define PIPE_CHANNEL 1                               /* PIPE                  */
#define SOCKET_CHANNEL 2                             /* SOCKET                */
#define FILE_CHANNEL 3                               /* FILE                  */
#define VELOCITY_VALUE_LEN 15
#define ERROR_VALUE_LEN 9
#define PATH_FILE_CHANNEL "./tmp/file_vel.tmp"
#define PATH_PIPE_CHANNEL "./tmp/pipe_vel.tmp"
#define PATH_SOCKET_CHANNEL "./tmp/socket_vel.tmp"
#define PATH_ERROR_CHANNEL "./tmp/pipe_err.tmp"
#define PATH_PFC01_PID "./tmp/pfc01_pid.tmp"
#define PATH_PFC02_PID "./tmp/pfc02_pid.tmp"
#define PATH_PFC03_PID "./tmp/pfc03_pid.tmp"
#define PATH_READER_PID "./tmp/reader_pid.tmp"
#define PATH_TRANS_PID "./tmp/trans_pid.tmp"
#define PATH_FAILG_PID "./tmp/failg_pid.tmp"
#define PATH_LAST_UUID_INS_PFC03 "./tmp/pfc03_last_uuid.tmp"
#define SPEED_PFC_01 "SPEED_PFC_01"
#define SPEED_PFC_02 "SPEED_PFC_02"
#define SPEED_PFC_03 "SPEED_PFC_03"
#define STATUS "STATUS"
#define FAILURE_SIGINT 10000                /* Probability of SIGINT  failure */
#define FAILURE_SIGSTOP 100                 /* Probability of SIGSTOP failure */
#define FAILURE_SIGCONT 10                  /* Probability of SIGCONT         */
#define FAILURE_SIGUSR1 10                  /* Probability of SIGUSR1 failure */
#define END_SPEED_STREAM "END_STREAM"
#define WES_STATUS_ALLOK "ALLOK:--"
#define WES_STATUS_ERROR "ERROR:"
#define WES_STATUS_EMERGENCY "EMERG:--"
#define SWITCH "SWITCH"

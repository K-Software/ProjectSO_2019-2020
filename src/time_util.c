/* -------------------------------------------------------------------------- */
/* time_util.c                                                                */
/* -------------------------------------------------------------------------- */
#include <errno.h>
#include <time.h>
#include "time_util.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */
/*
 * DESCRIPTION
 * Sleep for the requested number of milliseconds.
 *
 * PARAMETERS:
 * - msec = Numbers of milliseconds
 *
 * RETURN VALUES:
 * Upon successful completion, mssleep returns 0. Otherwise, return a value
 * different than 0.
 */
int mssleep(long msec)
{
  struct timespec ts;
  int res;

  if (msec < 0)
    {
      return -1;
    }

  ts.tv_sec = msec / 1000;      /* Numbers of seconds */
  ts.tv_nsec = (msec % 1000) * 1000000; /* Numbers of nanoseconds */

  do {
    res = nanosleep(&ts, &ts);
  } while (res && errno == EINTR);

  return res;
}

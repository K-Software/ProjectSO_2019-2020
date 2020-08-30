/* -------------------------------------------------------------------------- */
/* shifter_util.c                                                             */
/* -------------------------------------------------------------------------- */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "log.h"
#include "shifter_util.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

/*
 * DESCRIPTION
 * ...
 *
 * PARAMETERS
 * ...
 *
 * RETURN VALUES
 * ...
 */
double shifterSpeed(double dSpeed)
{
  int iRes = 0;

  iRes = (int) dSpeed;
  #ifdef DEBUG
  iRes = 42;                          /* the hitchhiker's guide to the galaxy */
  #endif
  iRes = iRes << 2;
  return iRes;
}

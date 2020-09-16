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
 * This function converts the speed contains inside dSpeed param from double 
 * to int and then shift left of 2 bit this value.
 *
 * PARAMETERS
 * - dSpeed = Speed
 *
 * RETURN VALUES
 * Return the new value of the speed.
 */
double shifterSpeed(double dSpeed)
{
  int iRes = 0;

  iRes = (int) dSpeed;                      /* Cast speed from double to int  */
  #ifdef DEBUG
  iRes = 42;                          /* the hitchhiker's guide to the galaxy */
  #endif
  iRes = iRes << 2;                         /* Shift the speed value of 2 bit */
  return iRes;
}

/* -------------------------------------------------------------------------- */
/* mathgps.c                                                                  */
/* -------------------------------------------------------------------------- */
#include <stdio.h>
#include <math.h>
#include "mathgps.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */
/* From https://nssdc.gsfc.nasa.gov/planetary/factsheet/earthfact.html */
#define EARTH_EQUATORIAL_RADIUS_M 6378137
#define EARTH_POLAR_RADIUS_M 6356752
#define DEGREES_TO_RADIANS (M_PI / 180)

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */

#ifdef DEBUG
int main(int argc, char *argv[])
{
  float lat;

  printf("Latitude:");
  scanf("%f",&lat);
  printf("Earth radius at latitude %.2f degeers: %.2f m\n", lat,
         earthRadius(lat, 0));
  printf("Distance from London to Arlington is %.2f m\n",
         distanceBetweenEarthCoordinates(51.5, 0, 38.8, -77.1));
}
#endif

/*
 * DESCRIPTION
 * This function calculate the radius of earth by latitude.
 * Formula for calcolation:
 * - B  = latitude;
 * - r1 = radius at equator;
 * - r2 = radius at pole;
 * - R  = √[(r1²*cos(B))²+(r2²*sin(B))²]/[(r1*cos(B))²+(r2*sin(B))²].
 *
 * PARAMETERS:
 * lat = latitude in decimal degrees;
 * metersASL = meters above sea level.
 *
 * RETURN VALUES:
 * Return the radius of the earth in meters for a specific latitude and
 * height above sea level.
 */
double earthRadius(double lat, int metersASL)
{
  double r1 = EARTH_EQUATORIAL_RADIUS_M;
  double r2 = EARTH_POLAR_RADIUS_M;
  double coslat = cos(lat * DEGREES_TO_RADIANS);
  double sinlat = sin(lat * DEGREES_TO_RADIANS);
  return sqrt((pow(pow(r1, 2) * coslat, 2) + pow(pow(r2, 2) * sinlat, 2))
              / (pow(r1 * coslat,2) + pow(r2 * sinlat,2))) + metersASL;
}

/*
 * DESCRIPTION
 * This function calculate the distance in meters between point A and point B.
 *
 * PARAMETERS:
 * - latA = latitude of point A in decimal degrees;
 * - lonA = longitude of point A in decimal degrees;
 * - latB = latitude of point B in decimal degrees;
 * - lonB = longitude of point B in decimal degrees.
 *
 * RETURN VALUES:
 * Return the distance in meters between point A and point B with the radius of
 * the earth calculated on the latitude of point B.
 */
double distanceBetweenEarthCoordinates(double latA, double lonA,
                                       double latB, double lonB)
{
  double dLat = (latB-latA) * DEGREES_TO_RADIANS;
  double dLon = (lonB-lonA) * DEGREES_TO_RADIANS;
  double latArad = latA * DEGREES_TO_RADIANS;
  double latBrad = latB * DEGREES_TO_RADIANS;
  double a = (sin(dLat/2) * sin(dLat/2)) +
    (sin(dLon/2) * sin(dLon/2) * cos(latArad) * cos(latBrad));
  double c = 2 * atan2(sqrt(a), sqrt(1-a));
  return earthRadius(latB, 0) * c;
  /* return 6369783.34 * c; */
}

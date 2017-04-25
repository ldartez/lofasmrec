
#ifndef MJD_H
#define MJD_H

#include <time.h>

const double MJD2000 = 51545.0; // MJD for January 1, 2000 00:00:00
const double MJD1970 = 40587.0; // MJD for Jan 1 1970 00:00:00
void mjd(double *, time_t);

#endif
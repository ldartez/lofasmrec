
#ifndef MJD_H
#define MJD_H

#include <time.h>
#include "boost/date_time/posix_time/posix_time.hpp"

const double MJD2000 = 51545.0; // MJD for January 1, 2000 00:00:00
const double MJD1970 = 40587.0; // MJD for Jan 1 1970 00:00:00

struct mjd_t {
    double days;
    double ms;
};

double mjd_datetime();
void mjd(mjd_t*, boost::posix_time::ptime);


#endif

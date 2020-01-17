#include "mjd.h"
#include <string>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

#define DAY_MICROSEC 86400000000
#define DAY_MILLISEC 86400000

double mjd_datetime() {
    boost::posix_time::ptime now = boost::posix_time::microsec_clock::universal_time();
    boost::gregorian::date d(now.date());
    // fractional day in milliseconds
    double day_us = (now.time_of_day()).total_microseconds() / DAY_MICROSEC;
    return d.modjulian_day() + day_us;
}

void mjd(mjd_t *m, boost::posix_time::ptime t)
{

    /*
     * calculate modified julian date from tm object.
     *
     * MJD is calculated by taking the number of days between t and Jan 1, 1970 00:00:00
     * and adding that value to the MJD for the epoch.
     *
     * Since we are using time_t, we are limited to 1 second precision here.
     *
     */

    boost::gregorian::date d(t.date());

    m->days = (int) d.modjulian_day(); // integer mjd days
    m->ms = (t.time_of_day()).total_milliseconds(); // number of milliseconds since midnight

}

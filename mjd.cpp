#include "mjd.h"
#include <string>




void mjd(mjd_t *m, time_t t)
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

    double msec_day = 86400000; // milliseconds in a day
    double s = (double) t;

    double days = s/86400.0;
    double nowmjd = MJD1970 + days;

    //mjdbuf[0] = (int) nowmjd; // integer mjd days
    //mjdbuf[1] = (nowmjd - mjdbuf[0]) * msec_day; // number of milliseconds since midnight
    m->days = (int) nowmjd; // integer mjd days
    m->ms = (nowmjd - m->days) * msec_day; // number of milliseconds since midnight


}
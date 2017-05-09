//
// Created by Louis Dartez on 3/5/17.
//

#include "fhdrv3.h"
#ifndef STRINGH_INCLUDE_GUARD
#define STRINGH_INCLUDE_GUARD
#include <string.h>
#endif

#ifndef IOSTREAM_INCLUDE_GUARD
#define IOSTREAM_INCLUDE_GUARD
#include <iostream>
#endif

#include "mjd.h"

void construct_hdrV4(char *buf, time_t t, int station_id, int Nsamp)
{
    /*
     * Construct header string for .lofasm data format
     *
     * The values for the header fields _should_ be parameters to this function
     * but since they haven't changed in a long time I'm writing them in for now.
     *
     * For V4 buf should have space for at least 96 characters.
     */

    double *mjdvals = new double [2]; // storage for the numeric mjd values
    char mjdday[9] = "        ";
    char mjdms[9] = "        ";
    char station[9] = "        ";
    char nsamples[9] = "        ";


    // calculate MJD values needed for the header
    mjd(mjdvals, t);

    // convert values to strings
    std::string day = std::to_string((int) mjdvals[0]); // this will be 5 characters long for many years
    std::string ms = std::to_string(mjdvals[1]);
    std::string station_str = std::to_string(station_id);
    std::string nsamp_str = std::to_string(Nsamp);

    strncpy(mjdms, ms.c_str(), ms.size());
    strncpy(mjdday, day.c_str(), day.size());

    //copy station id number into padded character array
    int offset = HDRENTRYLEN - station_str.size();
    strncpy(station+offset, station_str.c_str(), station_str.size());

    //copy nsamples into padded character array
    offset = HDRENTRYLEN - nsamp_str.size();
    strncpy(nsamples+offset, nsamp_str.c_str(), nsamp_str.size());


    strncpy(buf, HDRSIG, (size_t) HDRENTRYLEN);
    strncpy(buf+HDRENTRYLEN, HDRVER, (size_t) HDRENTRYLEN);
    strncpy(buf+2*HDRENTRYLEN, HDRLEN, (size_t) HDRENTRYLEN);
    strncpy(buf+3*HDRENTRYLEN, station, (size_t) HDRENTRYLEN);
    strncpy(buf+4*HDRENTRYLEN, HDRNBINS, (size_t) HDRENTRYLEN);
    strncpy(buf+5*HDRENTRYLEN, HDRFSTART, (size_t) HDRENTRYLEN);
    strncpy(buf+6*HDRENTRYLEN, HDRFSTEP,  (size_t) HDRENTRYLEN);
    strncpy(buf+7*HDRENTRYLEN, mjdday, (size_t) HDRENTRYLEN);
    strncpy(buf+8*HDRENTRYLEN, mjdms, (size_t) HDRENTRYLEN);
    strncpy(buf+9*HDRENTRYLEN, HDRINTTIME, (size_t) HDRENTRYLEN);
    strncpy(buf+10*HDRENTRYLEN, HDRFMTVER, (size_t) HDRENTRYLEN);
    strncpy(buf+11*HDRENTRYLEN, nsamples, (size_t) HDRENTRYLEN);

}

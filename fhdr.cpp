//
// Created by Louis Dartez on 3/5/17.
//

#include "fhdr.h"

#ifndef STRINGH_INCLUDE_GUARD
#define STRINGH_INCLUDE_GUARD
#include <string.h>
#endif

#ifndef IOSTREAM_INCLUDE_GUARD
#define IOSTREAM_INCLUDE_GUARD
#include <iostream>
#endif

#include "mjd.h"
#include "err.h"

void constructFileHeader(char *buf, time_t t, int header_version, int station_id, int Nsamp)
{
    /*
     * Construct header string for .lofasm data format
     *
     * The values for the header fields _should_ be parameters to this function
     * but since they haven't changed in a long time I'm writing them in for now.
     *
     * For V4 buf should have space for at least 96 characters.
     */

    //double *mjdvals = new double [2]; // storage for the numeric mjd values
    mjd_t *m = new mjd_t;
    char mjdday[9] = "        ";
    char mjdms[9] = "        ";
    char station[9] = "        ";
    char nsamples[9] = "        ";
    char hlen[9] = "        ";
    char hver[9] = "        ";
    int offset, offset_len, offset_ver;
    std::string hdrlen, hdrver;


    // calculate MJD values needed for the header
    mjd(m, t);

    // convert values to strings
    std::string day = std::to_string((int) m->days); // this will be 5 characters long for many years
    std::string ms = std::to_string(m->ms);
    std::string station_str = std::to_string(station_id);
    std::string nsamp_str = std::to_string(Nsamp);
   
    if (header_version == 3){
        hdrlen = std::to_string((int) 108);
        hdrver = std::to_string((int) header_version);
        offset_len = HDRENTRYLEN - hdrlen.size();
        offset_ver = HDRENTRYLEN - hdrver.size();
    }
    else if (header_version == 4){
        hdrlen = std::to_string((int) 96);
        hdrver = std::to_string((int) header_version);
        offset_len = HDRENTRYLEN - hdrlen.size();
        offset_ver = HDRENTRYLEN - hdrver.size();
    }
    strncpy(hlen+offset_len, hdrlen.c_str(), hdrlen.size());
    strncpy(hver+offset_ver, hdrver.c_str(), hdrver.size());

    strncpy(mjdms, ms.c_str(), ms.size());
    strncpy(mjdday, day.c_str(), day.size());

    //copy station id number into padded character array
    offset = HDRENTRYLEN - station_str.size();
    strncpy(station+offset, station_str.c_str(), station_str.size());

    //copy nsamples into padded character array
    offset = HDRENTRYLEN - nsamp_str.size();
    strncpy(nsamples+offset, nsamp_str.c_str(), nsamp_str.size());


    strncpy(buf, HDRSIG, (size_t) HDRENTRYLEN);
    strncpy(buf+HDRENTRYLEN, hver, (size_t) HDRENTRYLEN);
    strncpy(buf+2*HDRENTRYLEN, hlen, (size_t) HDRENTRYLEN);
    strncpy(buf+3*HDRENTRYLEN, station, (size_t) HDRENTRYLEN);
    strncpy(buf+4*HDRENTRYLEN, HDRNBINS, (size_t) HDRENTRYLEN);
    strncpy(buf+5*HDRENTRYLEN, HDRFSTART, (size_t) HDRENTRYLEN);
    strncpy(buf+6*HDRENTRYLEN, HDRFSTEP,  (size_t) HDRENTRYLEN);
    strncpy(buf+7*HDRENTRYLEN, mjdday, (size_t) HDRENTRYLEN);
    strncpy(buf+8*HDRENTRYLEN, mjdms, (size_t) HDRENTRYLEN);
    strncpy(buf+9*HDRENTRYLEN, HDRINTTIME, (size_t) HDRENTRYLEN);
    strncpy(buf+10*HDRENTRYLEN, HDRFMTVER, (size_t) HDRENTRYLEN);
    
    if (header_version == 3){
        strncpy(buf+11*HDRENTRYLEN, "          ", (size_t) 10);
        strncpy(buf+11*HDRENTRYLEN+10, "          ", (size_t) 10);
    }
    else if (header_version == 4){
        strncpy(buf+11*HDRENTRYLEN, nsamples, (size_t) HDRENTRYLEN);
    }
    else {
        error("unrecognized header format");
    }

}

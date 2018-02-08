//
// Created by Louis Dartez on 3/5/17.
//

#include "fhdr.h"
#include <string>
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
#include "cfg.h"
#include <cmath>
void constructFileHeader(std::string *buf, time_t t, Configuration cfg, int Nsamp)
{
     /*
     * Construct header string for .lofasm data format
     *
     * The values for the header fields _should_ be parameters to this function
     * but since they haven't changed in a long time I'm writing them in for now.
     *
     * For V4 buf should have space for at least 96 characters.
     */
    
    int header_version = cfg.hdr_version;
    int station_id = cfg.station_id;

    mjd_t *m = new mjd_t;
    char mjdday[HDRENTRYLEN+1] = "        ";
    char mjdms[HDRENTRYLEN+1] = "        ";
    char station[HDRENTRYLEN+1] = "        ";
    char nsamples[HDRENTRYLEN+1] = "        ";
    char hlen[HDRENTRYLEN+1] = "        "; // length of header string in bytes
    char hver[HDRENTRYLEN+1] = "        "; // header version
    int offset, offset_len, offset_ver;
    std::string hdrlen, hdrver;
    //std::string mjdday, mjdms, station, nsamples;
    //std::string hlen, hver;

    // calculate MJD values needed for the header
    mjd(m, t);

    // convert values to strings
    std::string day = std::to_string((int) m->days);
    std::string ms = std::to_string((int) round(m->ms));
    std::string station_str = std::to_string(station_id);
    std::string nsamp_str = std::to_string(Nsamp);

    // format values to correct size
    formatHeaderEntry(day);
    formatHeaderEntry(ms);
    formatHeaderEntry(station_str);
    formatHeaderEntry(nsamp_str);

    // set sizes for header length and version fields
    if (header_version == 3)
    {
        hdrlen = std::to_string((int) 108);
        hdrver = std::to_string((int) header_version);
        offset_len = HDRENTRYLEN - hdrlen.size();
        offset_ver = HDRENTRYLEN - hdrver.size();
    }
    else if (header_version == 4)
    {
        hdrlen = std::to_string((int) 96);
        hdrver = std::to_string((int) header_version);
        offset_len = HDRENTRYLEN - hdrlen.size();
        offset_ver = HDRENTRYLEN - hdrver.size();
    }
    else if (header_version == 5)
    {
	hdrlen = std::to_string((int) 128);
	hdrver = std::to_string((int) header_version);
	offset_len = HDRENTRYLEN - hdrlen.size();
	offset_ver = HDRENTRYLEN - hdrver.size();
    }

    // populate header length and version
    strncpy(hlen+offset_len, hdrlen.c_str(), hdrlen.size());
    strncpy(hver+offset_ver, hdrver.c_str(), hdrver.size());

    // copy mjd date
    strncpy(mjdms, ms.c_str(), ms.size());
    strncpy(mjdday, day.c_str(), day.size());

    //copy station id number into padded character array
    offset = HDRENTRYLEN - station_str.size();
    strncpy(station+offset, station_str.c_str(), station_str.size());

    //copy nsamples into padded character array
    offset = HDRENTRYLEN - nsamp_str.size();
    strncpy(nsamples+offset, nsamp_str.c_str(), nsamp_str.size());

    buf->append(HDRSIG);
    buf->append(hver);
    buf->append(hlen);
    buf->append(station);
    buf->append(HDRNBINS);
    buf->append(HDRFSTART);
    buf->append(HDRFSTEP);
    buf->append(mjdday);
    buf->append(mjdms);
    buf->append(HDRINTTIME);
    buf->append(HDRFMTVER);
    
    if (header_version == 3){
	// last two fields are 10 bytes long
	buf->append("          ");
	buf->append("          ");
    }
    else if (header_version == 4){
	buf->append(nsamples);
    }
    else if (header_version == 5)
    {
	buf->append(nsamples);
	
	// write trunk line labels.
	std::string trunkLabel;
	trunkLabel = cfg.trunkA;
	formatHeaderEntry(trunkLabel);
	buf->append(trunkLabel.c_str());
	trunkLabel = cfg.trunkB;
	formatHeaderEntry(trunkLabel);
	buf->append(trunkLabel.c_str());
	trunkLabel = cfg.trunkC;
	formatHeaderEntry(trunkLabel);
	buf->append(trunkLabel.c_str());
	trunkLabel = cfg.trunkD;
	formatHeaderEntry(trunkLabel);
	buf->append(trunkLabel.c_str());
	

    }
    else {
        error("unrecognized header format");
    }
}

void formatHeaderEntry(std::string &buf)
{
    /*
      fill buf with leading spaces
     */

    // cap the buffer size at HDRENTRYLEN.
    // if input buffer is larger than HDRENTRYLEN
    // then truncate.
    size_t l = buf.length();
    if (l > HDRENTRYLEN)
	l = HDRENTRYLEN;
    
    std::string *tmp = new std::string();

    // copy characters to tmp
    for (int i=0; i<l; i++)
    {
	*tmp += buf[i];
    }
    
    buf.clear();
    buf.insert(buf.begin(), HDRENTRYLEN-tmp->length(), ' ');
    buf += *tmp;
    delete tmp;
}

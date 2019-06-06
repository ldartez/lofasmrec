//
// Created by Louis Dartez on 3/5/17.
//

/*
 * constants relevant to the generation of the .lofasm file header V3.
 */

#ifndef PACKET_SNIFFER_FHDR_H
#define PACKET_SNIFFER_FHDR_H

#ifndef CTIME_INCLUDE_GUARD
#define CTIME_INCLUDE_GUARD
#include <ctime>
#endif

#include "cfg.h"
#include <string>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

const size_t HDRENTRYLEN = 8;
const char HDRSIG[HDRENTRYLEN+1] = "    LoCo";
const char HDRNBINS[HDRENTRYLEN+1] = "    2048";
const char HDRFSTART[HDRENTRYLEN+1] = "       0";
const char HDRFSTEP[HDRENTRYLEN+1] = "09765625";
const char HDRINTTIME[HDRENTRYLEN+1] = "0.083886"; 
const char HDRFMTVER[HDRENTRYLEN+1] = "       1";

const unsigned int HDRV3_LENGTH = 108;
const unsigned int HDRV4_LENGTH = 96;
const unsigned int HDRV5_LENGTH = 128;

void constructFileHeader(std::string *, boost::posix_time::ptime, Configuration, int);
void formatHeaderEntry(std::string&);
#endif //PACKET_SNIFFER_FHDR_H

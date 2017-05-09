//
// Created by Louis Dartez on 3/5/17.
//

/*
 * constants relevant to the generation of the .lofasm file header V4.
 */

#ifndef PACKET_SNIFFER_FHDRV4_H
#define PACKET_SNIFFER_FHDRV4_H

#ifndef CTIME_INCLUDE_GUARD
#define CTIME_INCLUDE_GUARD
#include <ctime>
#endif

const unsigned int HDRENTRYLEN = 8;
const char HDRSIG[HDRENTRYLEN+1] = "    LoCo";
const char HDRVER[HDRENTRYLEN+1] = "       4";
const char HDRLEN[HDRENTRYLEN+1] = "     96";
const char HDRSTATION[HDRENTRYLEN+1] = "       0"; //specific to each station, 0 is default.
const char HDRNBINS[HDRENTRYLEN+1] = "    2048";
const char HDRFSTART[HDRENTRYLEN+1] = "       0";
const char HDRFSTEP[HDRENTRYLEN+1] = "09765625";
const char HDRINTTIME[HDRENTRYLEN+1] = "0.083886";
const char HDRFMTVER[HDRENTRYLEN+1] = "       1";
const int HDRLENGTH = 96;

void construct_hdrV4(char *, time_t, int, int);

#endif //PACKET_SNIFFER_FHDRV3_H

//
// Created by Louis Dartez on 3/5/17.
//

/*
 * constants relevant to the generation of the .lofasm file header V3.
 */

#ifndef PACKET_SNIFFER_FHDRV3_H
#define PACKET_SNIFFER_FHDRV3_H

#ifndef CTIME_INCLUDE_GUARD
#define CTIME_INCLUDE_GUARD
#include <ctime>
#endif

const unsigned int HDRENTRYLEN = 8;
const char HDRSIG[HDRENTRYLEN+1] = "    LoCo";
const char HDRVER[HDRENTRYLEN+1] = "       3";
const char HDRLEN[HDRENTRYLEN+1] = "     108";
const char HDRSTATION[HDRENTRYLEN+1] = "       0"; //specific to each station, 0 is default.
const char HDRNBINS[HDRENTRYLEN+1] = "    2048";
const char HDRFSTART[HDRENTRYLEN+1] = "       0";
const char HDRFSTEP[HDRENTRYLEN+1] = "09765625";
const char HDRINTTIME[HDRENTRYLEN+1] = "0.083886";
const char HDRFMTVER[HDRENTRYLEN+1] = "       1";
const char HDRRA[11] = "          "; // to be populated later
const char HDRDEC[11] = "          "; // to be populated later
const int HDRLENGTH = 108;

void construct_hdrV3(char *, time_t, int);

#endif //PACKET_SNIFFER_FHDRV3_H

//
// Created by Louis Dartez on 3/5/17.
//

#ifndef PACKET_SNIFFER_LFREC_H
#define PACKET_SNIFFER_LFREC_H

#include "cfg.h"
#include <ctime>
#include <string>

void record_timed(float, Configuration);
std::string construct_filename(time_t, Configuration);
std::string fmt_val(int, size_t);

#endif //PACKET_SNIFFER_LFREC_H

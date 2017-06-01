//
// Created by Louis Dartez on 3/5/17.
//

#ifndef PACKET_SNIFFER_LFREC_H
#define PACKET_SNIFFER_LFREC_H

#include "cfg.h"


void record_timed(float, Configuration);
time_t construct_filename(char *, Configuration);
void fmt_char_val(char*, int, int, size_t);

#endif //PACKET_SNIFFER_LFREC_H

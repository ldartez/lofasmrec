//
// Created by Louis Dartez on 3/5/17.
//

#ifndef PACKET_SNIFFER_LFREC_H
#define PACKET_SNIFFER_LFREC_H

#include "cfg.h"
#include <ctime>
#include <cstdio>
#include <string>
#include <gzstream.h>
#include "boost/date_time/posix_time/posix_time.hpp"


// callback struct for pcap loop callback argument passing
struct callback_args {
    int id;
    unsigned char *payload;
    ogzstream *ofile;
};

void record_timed(float, Configuration);
void record_pcap(float, Configuration);
std::string construct_filename(boost::posix_time::ptime, Configuration);
std::string fmt_val(int, size_t);
void handle_packet(u_char *, const struct pcap_pkthdr *, const u_char *);

#endif //PACKET_SNIFFER_LFREC_H

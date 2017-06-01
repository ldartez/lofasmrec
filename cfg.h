//
// Created by Louis Dartez on 3/30/17.
//

#ifndef CFG_H
#define CFG_H

#ifndef STRING_INCLUDE_GUARD
#include <string>
#endif

#ifndef VECTOR_INCLUDE_GUARD
#define VECTOR_INCLUDE_GUARD
#include <vector>
#endif


// struct container for configuration parameters
struct cfgfield
{
    std::string name, value;
};

// class to represent the LoFASM recording configuration
class Configuration
{
public:
    std::string pidfile, dataroot, roach_ip, roach_ip_10gbe, incoming_ip_10gbe;
    int blocktime, station_id, roach_port, roach_port_10gbe;
    bool hdr_on;
    int hdr_version, rec_mode;
    int incoming_port_10gbe, n;
    int samp_len;
    float tsamp;
    int hdrlength;
    int blocksize, bbr_id;

    Configuration(const char *);

    void print();

private:
    const char *cfgfile;
    std::vector<cfgfield*>* cfgfields;

    int readcfg();
};

#endif

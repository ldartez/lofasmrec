//
// Created by Louis Dartez on 3/30/17.
//

#ifndef PACKET_SNIFFER_CFG_H
#define PACKET_SNIFFER_CFG_H

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

class Configuration
{
public:
    std::string pidfile, dataroot, roach_ip, roach_ip_10gbe, incoming_ip_10gbe;
    int blocktime, station_id, roach_port, roach_port_10gbe;
    int incoming_port_10gbe, n;

    Configuration(char *);
    void print();

private:
    char *cfgfile;
    std::vector<cfgfield*>* cfgfields;
    int readcfg();
};

#endif //PACKET_SNIFFER_CFG_H

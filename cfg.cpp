//
// Created by Louis Dartez on 3/30/17.
//

#include "cfg.h"
#include <fstream>

#ifndef IOSTREAM_INCLUDE_GUARD
#define IOSTREAM_INCLUDE_GUARD
#include <iostream>
#endif

#ifndef VECTOR_INCLUDE_GUARD
#define VECTOR_INCLUDE_GUARD
#include <vector>
#endif

#ifndef STRING_INCLUDE_GUARD
#define STRING_INCLUDE_GUARD
#include <string>
#endif

#include "err.h"


Configuration::Configuration(const char *infile)
{
    cfgfields = new std::vector<cfgfield*>;
    cfgfile = infile;

    n = readcfg();


    //populate member variables
    for (int i=0; i<n; i++)
    {
        cfgfield field = *(*cfgfields)[i];

        if (field.name == "pidfile")
        {
            pidfile = field.value;
        }
        else if (field.name == "dataroot")
        {
            dataroot = field.value;
        }
        else if (field.name == "blocktime")
        {
            blocktime = std::stof(field.value);
        }
        else if (field.name == "station_id")
        {
            station_id = std::stoi(field.value);
        }
        else if (field.name == "roach_ip")
        {
            roach_ip = field.value;
        }
        else if (field.name == "roach_ip_10gbe")
        {
            roach_ip_10gbe = field.value;
        }
        else if (field.name == "roach_port")
        {
            roach_port = std::stoi(field.value);
        }
        else if (field.name == "roach_port_10gbe")
        {
            roach_port_10gbe = std::stoi(field.value);
        }
        else if (field.name == "incoming_ip_10gbe")
        {
            incoming_ip_10gbe = field.value;
        }
        else if (field.name == "incoming_port_10gbe")
        {
            incoming_port_10gbe = std::stoi(field.value);
        }
        else if (field.name == "tsamp")
        {
            tsamp = std::stof(field.value);
        }
        else if (field.name == "samp_len")
        {
            samp_len = std::stoi(field.value);
        }
        else if (field.name == "hdr_on")
        {
            hdr_on = std::stoi(field.value);
        }
        else if (field.name == "hdr_version")
        {
            hdr_version = std::stoi(field.value);
            if (hdr_version == 3){
                hdrlength = 108;
            }
            else if (hdr_version == 4){
                hdrlength = 96;
            }
        }
        else if (field.name == "rec_mode")
        {
            rec_mode = std::stoi(field.value);
        }
        else if (field.name == "blocksize")
        {
            blocksize = std::stoi(field.value); // max size of each file in bytes
        }
        else if (field.name == "bbr_id")
        {
            bbr_id = std::stoi(field.value);
        }
    }
}


int Configuration::readcfg() {
    /*
        Read settings from a configuration file and append each cfgfield to vector cfgfields.

        Return number of fields processed.
    */

    std::ifstream cfg(cfgfile);

    int linecount = 0;

    if (!cfg.is_open()) {
        error("Could not open file for reading");
    }
    else
    {
        std::cout << "Reading configuration file: " << cfgfile << std::endl;
    }

    std::string fieldname, fieldval;

    /*
        Iterate over lines in config file and place them in the vector cfgfields.
    */
    while (cfg >> fieldname >> fieldval)
    {
        cfgfield* cfgline = new cfgfield;

        cfgline->name = fieldname;
        cfgline->value = fieldval;

        cfgfields->push_back(cfgline);

        linecount++;
    }

    return linecount;
}


void Configuration::print()
{
    /*
    print all configuration fields
    */

    std::cout << "=========== LoFASM Rec. Configuration ============\n";

    int n = cfgfields->size();
    for (int i=0; i<n; i++)
    {
        std::cout << (*cfgfields)[i]->name << ":\t";
        std::cout << (*cfgfields)[i]->value << std::endl;
    }

    std::cout << "==================================================\n";


}

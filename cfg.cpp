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


Configuration::Configuration(char *infile)
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

    int n = cfgfields->size();
    for (int i=0; i<n; i++)
    {
        std::cout << i+1 << ". " << (*cfgfields)[i]->name << ": ";
        std::cout << (*cfgfields)[i]->value << std::endl;
    }


}

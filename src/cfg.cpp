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
    //std::cout << "out of readcfg @ " << __FILE__ << ":" << __LINE__ << std::endl;
    //std::cout << "readcfg returned " << n << std::endl;
    //std::cout << "cfgfields length is " << cfgfields->size() << std::endl;

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
            blocktime = std::stoi(field.value);
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
            hdr_on = (bool) std::stoi(field.value);
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
            else if (hdr_version == 5){
                hdrlength = 128;
            }
            else
            {
                hdrlength = 0; // header version not supported
                std::cout << "Header Version: " << hdr_version;
                std::cout << " not supported!\n";
            }
        }
        else if (field.name == "rec_mode")
        {
            rec_mode = std::stoi(field.value);
        }
        else if (field.name == "blocksize")
        {
            // max size of each file in bytes
            blocksize = std::stoi(field.value);
        }
        else if (field.name == "bbr_id")
        {
            bbr_id = std::stoi(field.value);
        }
        else if (field.name == "trunkA")
            {
                trunkA = field.value;
            }
        else if (field.name == "trunkB")
            {
                trunkB = field.value;
            }
        else if (field.name == "trunkC")
            {
                trunkC = field.value;
            }
        else if (field.name == "trunkD")
            {
                trunkD = field.value;
            }
    } // end for loop
} // end Configuration definition

int Configuration::readcfg(){
    std::ifstream cfg(cfgfile);
    if (!cfg.is_open()){
        std::cout << "Unable to open config file." << std::endl;
    }
    else {
        std::cout << "Reading: " << cfgfile << std::endl;
    }

    int n_fields = 0;
    int bufsize = 256;
    char buf[bufsize];
    std::string line, fieldname, fieldval;
    bool inSection = false;
    std::string sectionName = "[lofasmrec]";
    std::string::size_type eq_pos;
    while (cfg.getline(buf, bufsize)){
        line = buf;

        // ignore commented lines
        if (line.empty() || std::string::npos != line.substr(0, 1).find('#')){
            continue; // to next line
        }

        if (!inSection){
            if (std::string::npos != line.find(sectionName)){
                inSection = true;
                continue;
            }
        }
        else {
            // check if another section has started
            if (std::string::npos != line.find('[') && std::string::npos != line.find(']')) {
                // new config section has started
                break;
            }

            eq_pos = line.find(" = ");
            if (eq_pos == std::string::npos) {
                // equal sign not found in this line. ignore line.
                std::cout << "Skipping line due to formatting: " << line << std::endl;
                continue;
            }
            else {
                fieldname = line.substr(0, eq_pos);
                fieldval = line.substr(eq_pos+3, line.size());
                n_fields++;
                std::cout << n_fields << ": " << fieldname << " = " << fieldval << std::endl;
                cfgfield* cfield = new cfgfield();
                cfield->name = fieldname;
                cfield->value = fieldval;
                cfgfields->push_back(cfield);
            }
        }
    }
    cfg.close();
    return n_fields;
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

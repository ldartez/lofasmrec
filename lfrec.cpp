
//
// Created by Louis Dartez on 3/5/17.
//
#ifndef IOSTREAM_INCLUDE_GUARD
#define IOSTREAM_INCLUDE_GUARD
#include <iostream>
#endif

#include <string>
#include <cstring>

#ifndef MATH_INCLUDE_GUARD
#define MATH_INCLUDE_GUARD
#include <math.h>
#endif

#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <fstream>
#include <vector>
#include <gzstream.h>

#include "lfrec.h"
#include "fhdr.h"
#include "mjd.h"
#include "err.h"
#include "cfg.h"
#include "obs.h"


using namespace std;


// Class Definition



void record_timed(float recdur, Configuration cfg)
{
    /*
    Record data for a specified amount of time before exiting.

    Parameters
    =========
        recdur: float
            number of seconds to record for
        cfg: Configuration object
*/

    float blocktime = cfg.blocktime;
    string ipaddr_in = cfg.incoming_ip_10gbe;
    int port_in = cfg.incoming_port_10gbe;
    string dataroot = cfg.dataroot;
    int station_id = cfg.station_id;
    bool hdr_on = cfg.hdr_on;
    int hdr_version = cfg.hdr_version;

    int sock, Nblocks, samp_len, nblk, ntotal;
    int Nsamp; // filterbank samples in each block
    int Npkts; // network packets in each block
    ssize_t n;
    string lfhdr;
    string fname;
    string date_dirname;
    char buf[8192]; // buffer to hold socket data
    int Npkts_lastblock; // placeholder for number of packets in the last block
    int recdur_lastblock; //placeholder for duration (in seconds) of last block
    struct stat statbuf; // buffer for file stats
    int statcode; // return code from stat()
    socklen_t addr_length;
    struct sockaddr_in server;

    in_port_t port = (in_port_t) port_in;
    const char *ip_addr = ipaddr_in.c_str();

    time_t t; // timestamp for beginning of current file
    float tsamp; // filterbank sampling time

    int HDRLENGTH;
    if (hdr_version == 3){
        HDRLENGTH = HDRV3_LENGTH;
    }
    else if (hdr_version == 4)
    {
        HDRLENGTH = HDRV4_LENGTH;
    }
    else if (hdr_version == 5)
    {
        HDRLENGTH = HDRV5_LENGTH;
    }
    else
    {
        cout << "Error: header version not recognized.\n";
        exit(1);
    }


    // convert dataroot into char array
    //std::vector<char> fp(dataroot.size()+50);
    //std::strcpy(&fp[0], dataroot.c_str());
    //char *fpath = &fp[0];
    string fpath;

    Observation obs = Observation(recdur, cfg);
    Nblocks = obs.Nblocks;
    Npkts = obs.Npkts;
    Nsamp = Npkts / 17;
    Npkts_lastblock = obs.Npkts_lastblock;


    // create inet socket
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        perror("Opening socket");
        exit(0);
    }
    else cout << "socket opened.\n";

    addr_length = sizeof(server);

    // initialize with zeros..not sure why we have to do
    // this but it won't work otherwise
    memset(&server, 0, addr_length);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip_addr);
    server.sin_port = htons(port);

    // bind to socket address and port
    int bind_res = bind(sock, (struct sockaddr *)&server, addr_length);
    if (bind_res == -1) {
        cout << "socket addr: " << ip_addr << endl;
        cout << "socket port: " << port << endl;
        perror("error: binding");
        exit(0);
    }
    else
    {
        cout << "bind exit code: " << bind_res << endl;
        cout << "socket addr: " << ip_addr << endl;
        cout << "socket port: " << port << endl;
        cout << "binding successful\n";
        cout << "N blocks: " << Nblocks << endl;
        cout << "N Pkts per block: " << Npkts << endl;
        cout << "N samples per block: " << Nsamp << endl;
    }

    ntotal = 0; // total number of bytes received from socket
    for (int i=0; i<Nblocks; i++) // block loop
    {
        nblk = 0;

        // check if we need a special final block
        if (i == Nblocks-1 && Npkts_lastblock)
        {
            Npkts = Npkts_lastblock;
            Nsamp = Npkts/17;
        }

        // create filename
        t = time(nullptr);

        // get filename in format YYYYMMDD_HHMMSS.<extension>
        fname = construct_filename(t, cfg);
        date_dirname = fname.substr(0, 8);
        //strncpy(fpath+dataroot.size(), fname.c_str(), fp.size()-dataroot.size());

        // check if date_dirname already exists
        fpath = dataroot + date_dirname;
        statcode = stat(fpath.c_str(), &statbuf);

        // if directory doesn't exist then create it.
        // NOTE: I'm not checking if the directory path points
        // to a regular file. I don't expect that to ever happen.
        if (statcode == -1)
            {
                mkdir(fpath.c_str(), S_IRWXU | S_IRWXG | S_IRWXO);
            }

        // append filename to filepath.
        fpath += "/";
        fpath += fname;

        cout << i+1 << "/" << Nblocks << ": ";
        cout << fpath << endl;


        // open output file
        ogzstream ofile (fpath.c_str(), ios::out | ios::binary);

        // validate file opening
        if (!ofile.good())
        {
            error("unable to open file.");
        }

        if (hdr_on)
        {
            constructFileHeader(&lfhdr, t, cfg, Nsamp);
            ofile << lfhdr; // write formatted ascii header
        }


        // packet loop
        for (int k=0; k<Npkts; k++)
            {
                n = recv(sock, buf, 8192, 0);
                if (n<0)
                    {
                        std::cout << "error: recv\n";
                        ofile.close();
                        exit(1);
                    }
                 ofile.write(buf, (size_t) n);
                 nblk += n;
            }

        ntotal += nblk;
        ofile.close();
    }
}

string construct_filename(time_t t, Configuration cfg)
{
/*
    Construct LoFASM data file name using current date.
    Filename is stored in fname as a character array.
    The current time is returned as a time_t object.
*/
    string fname;
    fname.reserve(30);
    tm *gm = gmtime(&t);
    fname += fmt_val((int) 1900+gm->tm_year, 4);
    fname += fmt_val((int) 1+gm->tm_mon, 2);
    fname += fmt_val((int) gm->tm_mday, 2);
    fname += "_";
    fname += fmt_val((int) gm->tm_hour, 2);
    fname += fmt_val((int) gm->tm_min, 2);
    fname += fmt_val((int) gm->tm_sec, 2);

    if (cfg.rec_mode == 1) // bbr mode
        {
            fname += "_";
            fname += to_string(cfg.bbr_id);
            fname += ".bbr";
        }
    else if (cfg.rec_mode == 2) // spectrometer mode
        {
            fname += ".lofasm";
        }

    fname += ".gz";

    return fname;
}

string fmt_val(int val, size_t size)
{
    /*
      format val to be a string with _size_ characters.
      If val is longer than _size_ then truncate.
      Otherwise pad with leading zeros ("0").

      returns a string
    */

    string result;
    string sval = to_string(val);
    size_t sval_len, ndif;
    sval_len = sval.length();

    if (size > sval_len) // zero padding is needed
        {
            ndif = size - sval_len; // number of leading zeros to pad with
            for (int i=0; i<ndif; i++)
                {
                    result += "0";
                }
            result += sval;
        }
    else if (size < sval_len) // sval is too large
        {
            // truncate to size
            result = sval.substr(0, size);
        }
    else // sval is exactly the right size
        {
            result = sval;
        }

    return result;
}

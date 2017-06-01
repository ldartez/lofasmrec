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
#include <arpa/inet.h>
#include <fstream>
#include <vector>
#include "gzstream.h"

#include "lfrec.h"
#include "fhdr.h"
#include "mjd.h"
#include "err.h"
#include "cfg.h"
#include "obs.h"


using std::cout;
using std::endl;
using std::string;


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

    //int sock, nblk, btotal;
    int sock, Nblocks, samp_len, nblk, ntotal;
    int Nsamp; // filterbank samples in each block
    int Npkts; // network packets in each block
    ssize_t n;
    char lfhdr[109];
    char fname[30];
    char buf[8192]; // buffer to hold socket data
    int Npkts_lastblock; // placeholder for number of packets in the last block
    int recdur_lastblock; //placeholder for duration (in seconds) of last block

    socklen_t addr_length;
    struct sockaddr_in server;

    in_port_t port = (in_port_t) port_in;
    const char *ip_addr = ipaddr_in.c_str();

    time_t t;
    float tsamp; // filterbank sampling time

    int HDRLENGTH;
    if (hdr_version == 3){
        HDRLENGTH = 108;
    }
    else {
        HDRLENGTH = 96;
    }

    // convert dataroot into char array
    std::vector<char> fp(dataroot.size()+50);
    std::strcpy(&fp[0], dataroot.c_str());
    char *fpath = &fp[0];

    Observation obs = Observation(recdur, cfg);
    Nblocks = obs.Nblocks;
    Npkts = obs.Npkts;
    Nsamp = Npkts / 17; 
    Npkts_lastblock = obs.Npkts_lastblock;


    // create inet socket
    //sock = socket(PF_INET, SOCK_DGRAM, 0);
    sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (sock < 0) {
        perror("Opening socket");
        exit(0);
    }
    else cout << "socket opened.\n";

    addr_length = sizeof(server);
    memset(&server, 0, addr_length); // initialize with zeros..not sure why we have to do this but it won't work otherwise
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

        t = construct_filename(fname, cfg); // load filename into fname and corresponding time_t into t

        strncpy(fpath+dataroot.size(), fname, fp.size()-dataroot.size());

        cout << "new file: " << fpath << endl;


        // open output file
        std::ofstream ofile (fpath, std::ofstream::binary);
        //ogzstream ofile(fpath);

        if (!ofile.is_open())
        {
            error("unable to open file.");
        }


        if (hdr_on)
        {
            constructFileHeader(lfhdr, t, hdr_version, station_id, Nsamp);

            // write file header
            ofile.write(lfhdr, HDRLENGTH);
        }

        for (int k=0; k<Npkts; k++)
        { // packet loop
            n = recv(sock, buf, 8192, 0);
            if (n<0)
            {
                std::cout << "error: recv\n";
                ofile.close();
                exit(1);
            }
            ofile.write(buf, n);
            //ofile << buf;
            nblk += n;
        }

        ntotal += nblk;
        ofile.close();
    }
}


time_t construct_filename(char *fname, Configuration cfg)
{
/*
    Construct LoFASM data file name using current date.
    Filename is stored in fname as a character array.
    The current time is returned as a time_t object.
*/

    time_t t = std::time(nullptr);
    tm *gm = std::gmtime(&t);
    int ind;

    fmt_char_val(fname, 1900+gm->tm_year, 0, (size_t) 4);
    fmt_char_val(fname, 1+gm->tm_mon, 4 , (size_t) 2);
    fmt_char_val(fname, gm->tm_mday, 6, (size_t) 2);
    strncpy(fname+8, "_", (size_t) 1);
    fmt_char_val(fname, gm->tm_hour, 9, (size_t) 2);
    fmt_char_val(fname, gm->tm_min, 11, (size_t) 2);
    fmt_char_val(fname, gm->tm_sec, 13, (size_t) 2);
    //strcpy(fname+15, ".lofasm.gz");

    if (cfg.rec_mode == 1)
    {
        strncpy(fname+15, "_", (size_t) 1);
        strncpy(fname+16, std::to_string(cfg.bbr_id).c_str(), (size_t) 1);
        strcpy(fname+17, ".bbr");
    }
    else if (cfg.rec_mode == 2)
    {
        strcpy(fname+15, ".lofasm");
    }

    return t;
}

void fmt_char_val(char *buf, int val, int offset, size_t size)
{
/*
    format buf string to have _size_ bytes. If buf is longer than _size_
    then truncate. Otherwise pad with leading zeros ("0").
*/
    std::string sval = std::to_string(val);
    size_t l, dif;
    char *bufp = buf+offset;

    l = sval.length();

    if (size > l) // zero padding is needed
    {
        dif = size - l; // number of leading zeros to pad with
        strncpy(bufp, "0", dif);

    }
    else
    {
        dif = 0;
    }

    bufp += dif;

    strncpy(bufp, std::to_string(val).c_str(), size-dif);

}

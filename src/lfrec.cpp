
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


#include <fstream>
#include <vector>
#include <gzstream.h>
#include <pcap.h>
#include <cstdio>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <iomanip>
#include "lfrec.h"
#include "fhdr.h"
#include "mjd.h"
#include "err.h"
#include "cfg.h"
#include "obs.h"
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/gregorian/gregorian.hpp"

using namespace std;
namespace pt = boost::posix_time;
namespace gd = boost::gregorian;



void record_timed(float recdur, Configuration cfg) {
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

    pt::ptime t; // timestamp for beginning of current file
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
        t = pt::microsec_clock::universal_time();

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
            ofile.flush();
            lfhdr.clear();
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

void record_pcap(float recdur, Configuration cfg) {
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


    /* PCAP vars */
    char *dev = (char *) cfg.dev.c_str();
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *pcap_handle; // pcap session handle
    struct bpf_program pcap_bpf; // compiled filter expression
    char pcap_filter_exp[] = "udp port 60001"; // filter expression
    bpf_u_int32 mask, net; // netmask and ip of device
    struct pcap_pkthdr pcap_header; // pcap header
    const u_char *packet; // pcap packet data
    struct callback_args cb_args;


    /* PCAP SETUP */
    cout << "Device: " << dev << endl;
    if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
        cerr << "Can't get netmask for device " << dev << endl;
        net = 0;
        mask = 0;
    }
    pcap_handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
    if ( pcap_handle == NULL ) {
        cerr << "Couldn't open device: " << dev << ": " << errbuf << endl;
        exit(1);
    }
    // check link-layer header type
    if (pcap_datalink(pcap_handle) != DLT_EN10MB) {
        cerr << "Device " << dev << " doesn't provide Ethernet headers - not supported" << endl;
        exit(1);
    }
    if (pcap_compile(pcap_handle, &pcap_bpf, pcap_filter_exp, 0, net) == -1) {
        cerr << "Couldn't parse filter " << pcap_filter_exp << " : " << pcap_geterr(pcap_handle) << endl;
        exit(1);
    }
    if (pcap_setfilter(pcap_handle, &pcap_bpf) == -1) {
        cerr << "Couldn't install filter " << pcap_filter_exp << " : " << pcap_geterr(pcap_handle) << endl;
    }
    cout << "Done setting up pcap settings.\n";


    pt::ptime t; // timestamp for beginning of current file
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

    string fpath;
    Observation obs = Observation(recdur, cfg);
    Nblocks = obs.Nblocks;
    Npkts = obs.Npkts;
    Nsamp = Npkts / 17;
    Npkts_lastblock = obs.Npkts_lastblock;



    cout << "N blocks: " << Nblocks << endl;
    cout << "N Pkts per block: " << Npkts << endl;
    cout << "N samples per block: " << Nsamp << endl;

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

        // get timestamp using Boost library
        t = pt::microsec_clock::universal_time();

        // get filename in format YYYYMMDD_HHMMSS.<extension>
        fname = construct_filename(t, cfg);
        date_dirname = fname.substr(0, 8);

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
        // register output file in callback struct
        cb_args.ofile = &ofile;
        if (hdr_on)
        {
            constructFileHeader(&lfhdr, t, cfg, Nsamp);
            ofile << lfhdr; // write formatted ascii header
            ofile.flush();
            lfhdr.clear();
        }

        pcap_loop(pcap_handle, Npkts, handle_packet, (u_char *) &cb_args);
        ntotal += nblk;
        ofile.close();
    }
}

/* pcap callback packet handler */
void handle_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
    callback_args *cb_args = (callback_args *) args;
    if ( header->len > 0 ) {
        ogzstream *ofile = cb_args->ofile;
        cb_args->payload = (u_char *) (packet + 42);
        ofile->write((const char *) cb_args->payload, (size_t) 8192);
    }
    else {
        cerr << "empty packet?\n";
    }
}
string construct_filename(pt::ptime t, Configuration cfg) {
    /*
    Construct LoFASM data file name using current date.
    Filename is stored in fname as a character array.
    The current time is returned as a time_t object.
    */
    stringstream fname;

    gd::date date = t.date(); // get the date portion of the timestamp
    auto ymd = date.year_month_day();
    pt::time_duration tod = t.time_of_day();

    fname << ymd.year;
    fname << setw(2) << setfill('0') << (int) ymd.month;
    fname << setw(2) << setfill('0') << ymd.day;
    fname << '_';
    fname << setw(2) << setfill('0') << tod.hours();
    fname << setw(2) << setfill('0') << tod.minutes();
    fname << setw(2) << setfill('0') << tod.seconds();
    
    if (cfg.rec_mode == 1) // bbr mode
        {
            fname << "_";
            fname << to_string(cfg.bbr_id);
            fname << ".bbr";
        }
    else if (cfg.rec_mode == 2) // spectrometer mode
        {
            fname << ".lofasm";
        }

    fname << ".gz";

    return fname.str();
}

string fmt_val(int val, size_t size) {
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


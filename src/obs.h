
#ifndef OBS_H
#define OBS_H

#include "cfg.h"


class Observation
{
public:
    float tsamp, blocktime;
    int samp_len, Nblocks, Npkts, duration;
    unsigned int Nsamp;
    int recdur_lastblock, Npkts_lastblock;
    unsigned long int fsize;
    int rec_mode;

    Observation(int recdur, Configuration cfg);
    void print();

};

#endif

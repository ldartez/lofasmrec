
#ifndef OBS_H
#define OBS_H

#include "cfg.h"


class Observation
{
public:
    float tsamp, blocktime;
    int samp_len, Nblocks, Nsamp, Npkts, duration;
    int recdur_lastblock, Npkts_lastblock;
    unsigned long int fsize;

    Observation(int recdur, Configuration cfg);
    void print();

};

#endif

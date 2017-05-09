#include "obs.h"
#include "cfg.h"
#include "fhdrv3.h"
#include <math.h>


#ifndef IOSTREAM_INCLUDE_GUARD
#define IOSTREAM_INCLUDE_GUARD
#include <iostream>
#endif

Observation::Observation(int recdur, Configuration cfg)
{

        duration = recdur;
        blocktime = cfg.blocktime;
        tsamp = cfg.tsamp;
        samp_len = cfg.samp_len;

        // calculate number of blocks (files)
        if (recdur < blocktime || recdur == blocktime) // total duration is less than default blocktime
        {
            blocktime = duration;
            Nblocks = 1;
        }
        else if (fmod(duration,blocktime) == 0) // total duration is exact multiple of default blocktime
        {
            Nblocks = (int) duration/blocktime;
        }
        else // total duration is not a multiple of default blocktime. the last block will only contain the leftover data.
        {
            Nblocks = (int) duration/blocktime;
            recdur_lastblock = fmod(duration,blocktime);
            Npkts_lastblock = ((int) floor(recdur_lastblock / tsamp) + 1) * 17;
        }

        //calculate number of samples per block
        Nsamp = ((int) floor(blocktime / tsamp) + 1); // always get just one extra sample

        Npkts = Nsamp * 17; // total number of UDP packets per block

        fsize = Nsamp*samp_len+HDRLENGTH;


}

void Observation::print()
{
    std::cout << "======= Observation Report ===========\n";
    std::cout << "Duration: " << duration << " seconds.\n";
    std::cout << "Number of Files: " << Nblocks << std::endl;
    std::cout << "Samples Per File: " << Nsamp << std::endl;
    std::cout << "File size: " << fsize << " bytes\n";
    std::cout << "Total size: " << fsize*Nblocks << " B";
    std::cout << " | " << fsize*Nblocks/(pow(2,20)) << " MB | ";
    std::cout << fsize*Nblocks/pow(2,30) << " GB\n";
    std::cout << "======================================\n";
}

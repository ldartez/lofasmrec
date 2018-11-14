#include "obs.h"
#include "cfg.h"
#include "fhdr.h"
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
        rec_mode = cfg.rec_mode;


        if (rec_mode == 1) // baseband recording mode
        {


             // calculate number of blocks (files)

	    // total duration is less than default blocktime
            if (recdur*8192 < cfg.blocksize || recdur == cfg.blocksize)
            {
                cfg.blocksize = recdur*8192;
                Nblocks = 1;
            }
	    
	    // total duration is exact multiple of default blocktime
            else if (fmod(duration,cfg.blocksize) == 0)
            {
                //Nblocks = (int) duration/cfg.blocksize;
                ;
            }

	    // total duration is not a multiple of default blocksize.
	    //the last block will only contain the leftover data.
            else
            {
                //Nblocks = (int) duration/cfg.blocksize;
                recdur_lastblock = fmod(duration,cfg.blocksize);
                Npkts_lastblock = (int) (recdur_lastblock / 8192 ) + 1;
            }

            Nblocks = (int) ceil((float) (duration* 8192 ) / cfg.blocksize) ;
            Nsamp = (int) (cfg.blocksize / 4 );
            Npkts = (int) (cfg.blocksize / 8192);
            fsize = cfg.blocksize;


        }
        else if (rec_mode == 2) // spectrometer recording mode
        {

            // calculate number of blocks (files)
	  
	    // total duration is less than default blocktime
            if (recdur < blocktime || recdur == blocktime)
            {
                blocktime = duration;
                Nblocks = 1;
            }
	    
	    // total duration is exact multiple of default blocktime
            else if (fmod(duration,blocktime) == 0)
            {
                Nblocks = (int) duration/blocktime;
            }

	    // total duration is not a multiple of default blocktime.
	    //the last block will only contain the leftover data.
            else
            {
                Nblocks = (int) duration/blocktime;
                recdur_lastblock = fmod(duration,blocktime);
                Npkts_lastblock = ((int) floor(recdur_lastblock/tsamp) + 1) * 17;
            }

            //calculate number of samples per block
	    // always get just one extra sample
            Nsamp = ((unsigned int) floor(blocktime / tsamp) + 1);

            Npkts = Nsamp * 17; // total number of UDP packets per block

            fsize = Nsamp*samp_len + cfg.hdrlength;
        }

}

void Observation::print()
{
    if (rec_mode==1)
    {
        std::cout << "======= Baseband Observation ===========\n";
        std::cout << "No. Packets: " << duration << ".\n";
    }
    else if (rec_mode==2)
    {
        std::cout << "======= Spectrometer Observation ===========\n";
        std::cout << "Duration: " << duration << " seconds.\n";
    }
    std::cout << "Number of Files: " << Nblocks << std::endl;
    std::cout << "Samples Per File: " << Nsamp << std::endl;
    std::cout << "File size: " << fsize << " B | " << fsize/pow(2,20);
    std::cout << " MB | " << fsize/pow(2,30) << " GB\n";
    std::cout << "Total size: " << fsize*Nblocks << " B";
    std::cout << " | " << fsize*Nblocks/(pow(2,20)) << " MB | ";
    std::cout << fsize*Nblocks/pow(2,30) << " GB\n";
    std::cout << "======================================\n";
}

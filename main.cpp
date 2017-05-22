#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pwd.h> // getpwuid(), geteuid()

#ifndef STRING_INCLUDE_GUARD
#define STRING_INCLUDE_GUARD
#include <string>
#endif

#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>

#ifndef IOSTREAM_INCLUDE_GUARD
#define IOSTREAM_INCLUDE_GUARD
#include <iostream>
#endif

#ifndef CTIME_INCLUDE_GUARD
#define CTIME_INCLUDE_GUARD
#include <ctime>
#endif

#ifndef MATH_INCLUDE_GUARD
#define MATH_INCLUDE_GUARD
#include <math.h>
#endif

#ifndef VECTOR_INCLUDE_GUARD
#define VECTOR_INCLUDE_GUARD
#include <vector>
#endif

#include <iomanip>
#include <chrono>
#include <string>

// LoFASM header files
#include "fhdrv3.h" // LoFASM file header version 3 routines
#include "lfrec.h"  // LoFASM recording routines
#include "cfg.h"    // LoFASM configuration parsing routines
#include "err.h"    // functions to print error messages
#include "obs.h"


using std::cout;
using std::endl;
using std::vector;

// function prototypes
void error(const char*);
void print_usage(char *);
char* get_default_cfg();
void print_summary(float, Configuration);

// global variables


// main function definition
int main(int argc, char *argv[]) {
    bool tflag = false; // timed recording set
    bool oflag = false; // output directory set
    bool usercfg = false; // user-defined config
    bool printSummary = false;
    char *outputdir = nullptr;
    float tin;
    int c;
    Configuration* cfg;

    // config parameters
    std::string pidfname, dataroot;
    int blocktime;
    int pid = -1;
    int argindex;


    opterr = 0;


    /*
        Command line options

        t: number of seconds to record for
        c: specify configuration file to read. if not provided then default is used.
    */
    while ((c = getopt(argc, argv, "c:hl")) != -1)
    {
        switch (c)
        {

            case 'c':
            {
                std::cout << "Using config file: " << (std::string) optarg << std::endl;
                usercfg = true;
                cfg = new Configuration(optarg);
                break;
            }

            case 'h':
            {
                print_usage(argv[0]);
                exit(0);
                break;
            }

            case 'l':
            {
                printSummary = true;
                break;
            }

            case '?':
            {
                if (optopt == 'c')
                {
                    std::cout << "Option " << (char) optopt << " requires an argument.\n";
                }
                else if ( isprint(optopt) )
                {
                    std::cout << "Unknown option " << (char) optopt << std::endl;
                }
                else
                {
                    fprintf(stderr,
                        "Unknown option character '\\x%x'.\n",
                        optopt);
                }
                print_usage(argv[0]);
                exit(0);
            }
        }
    }

    if (!usercfg)
    {

         const char *cfg_path = "/home/controller/.lofasm/lofasm.cfg";
        cout << "Using default configuration file: /home/controller/.lofasm/lofasm.cfg\n";
        cfg = new Configuration(cfg_path);

    }


    // set tin if found in command line, else print usage and exit
    if (optind == argc-1)
    {
        tin = std::atof(argv[optind]);
    }
    else
    {
        print_usage(argv[0]);
        exit(0);
    }

    // print observation summary for debugging
    if (printSummary)
    {
        print_summary(tin, *cfg);
        Observation obs = Observation(tin, *cfg);
        obs.print();
        exit(0);
    }

    record_timed(tin, *cfg);
    exit(0);
}


// function definitions

void print_usage(char *pname) {
    cout << "Usage: " << pname << " [-h] [-l] [-c config_file] recdur\n";
    cout << "Options: \n";
    cout << "\t-c config_file: Path to configuration file to use.\n";
    cout << "\t-h: Print this menu and exit.\n";
    cout << "\t-l: Print job summary and exit.\n";
}


void print_summary(float tin, Configuration cfg)
{
    cfg.print();
}

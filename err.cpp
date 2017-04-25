//
// Created by Louis Dartez on 3/31/17.
//

#include "err.h"
#include <stdlib.h>
#include <stdio.h>


void error(const char *msg)
{
    perror(msg);
    exit(0);
}
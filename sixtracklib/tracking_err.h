//SixTrackLib
//
//Authors: R. De Maria, G. Iadarola, D. Pellegrini, H. Jasim
//
//Copyright 2017 CERN. This software is distributed under the terms of the GNU
//Lesser General Public License version 2.1, copied verbatim in the file
//`COPYING''.
//
//In applying this licence, CERN does not waive the privileges and immunities
//granted to it by virtue of its status as an Intergovernmental Organization or
//submit itself to any jurisdiction.

#ifndef _TRACKING_ERR_
#define _TRACKING_ERR_

#define Check_Failure(x) \
    if(err < 0) { \
        perror(x); \
        return NULL; \
    }

//#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#define _D
#define _DP(...) printf (__VA_ARGS__)
#else
//int printf(const char *format, ...);
#define _D for(;0;)
// #define _DP(...)
#endif

#endif

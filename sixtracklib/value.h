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


#ifndef _VALUE_
#define _VALUE_

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

#ifdef __OPENCL_VERSION__
  #define CLGLOBAL __global
  #define CLKERNEL __kernel
  #if __OPENCL_VERSION__ <= CL_VERSION_1_1
    #pragma OPENCL EXTENSION cl_khr_fp64 : enable
  #endif
  #define _GPUCODE
  #define GLOBAL_INDEX get_global_id(0)
  #define _CUDA_HOST_DEVICE_
#else
#ifdef __NVCC__
  #define _CUDA_HOST_DEVICE_ __device__
  #define CLGLOBAL
  #define CLKERNEL __global__
  #define _GPUCODE
  #define GLOBAL_INDEX threadIdx.x
#else
  #define CLGLOBAL
  #define CLKERNEL
  #define _CUDA_HOST_DEVICE_
  #define __constant
#endif
#endif


typedef unsigned long int uint64_t;
typedef signed long int int64_t;


typedef signed char byte_t;
typedef double real_t;
typedef signed long int integer_t;


typedef union value_t {
  double f64;
  long signed int i64;
  long unsigned int u64;
  //unsigned int u32;
} value_t;


#endif


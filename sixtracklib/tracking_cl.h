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

#ifndef _TRACKING_CL_
#define _TRACKING_CL_

#include "tracking_beam.h"
#include "tracking_elements.h"
#include "value.h"

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

#define Kernel_File "block.c"
#define Build_Includes "-I../common -I."
#define Tracking_Function "Block_track"
#define Track_By_Turn_Flag "-DTRACK_BY_TURN"
#define Track_By_Element_Flag "-DTRACK_BY_ELEMENT"


typedef struct {
    tracking_elements *t_elements;
    tracking_beam *t_beam;
    tracking_beam *t_beam_by_turn;
    tracking_beam *t_beam_by_element;
    cl_mem t_elements_buffer;
    cl_mem t_offsets_buffer;
    cl_mem t_beam_buffer;
    cl_mem t_beam_by_turn_buffer;
    cl_mem t_beam_by_element_buffer;
    cl_context t_context;
    cl_command_queue t_queue;
    cl_program t_program;
    cl_kernel t_kernel;
    unsigned int t_nturn;
} tracking_cl;

tracking_cl* tracking_cl_prepare(tracking_elements *elements, tracking_beam *beam, unsigned int nturn, int track_by_turn, int track_by_element);
cl_int tracking_cl_execute(tracking_cl *track);
cl_int tracking_cl_read(tracking_cl *track);
void tracking_cl_clean(tracking_cl *track);

#undef CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif

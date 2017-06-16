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


#ifndef _BLOCK_
#define _BLOCK_

#include "tracking_beam.h"
#include "value.h"

typedef enum type_t {
             DriftID, DriftExactID,
             MultipoleID, CavityID, AlignID,
             LinMapID,BB4DID,
             BlockID
} type_t;

#ifdef _GPUCODE

CLKERNEL void Block_track(
                 __constant value_t *elements_data, __constant unsigned int *elements_offsets, CLGLOBAL Particle *particles,
                 unsigned int nturn, unsigned int npart
                 #ifdef TRACK_BY_TURN
                   , CLGLOBAL Particle *particles_by_turn
                 #endif
                 #ifdef TRACK_BY_ELEMENT
                   , CLGLOBAL Particle *particles_by_element
                 #endif
);

#else

void Block_track(value_t *elements_data, unsigned int *elements_offsets, tracking_beam *beam,
                unsigned int nturn, unsigned int npart,
                Particle *particles_by_turn, Particle *particles_by_element);


#endif


#endif

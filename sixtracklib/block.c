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


#include "block.h"

#define DATA_PTR_IS_OFFSET
#include "../common/track.h"


// Tracking single

//#ifndef _GPUCODE
//#include <stdio.h>
//#endif

_CUDA_HOST_DEVICE_
int track_single(__constant value_t *elem,
                 CLGLOBAL Particle *p) {
   //if (p->state >= 0 ) {
       enum type_t typeid = (type_t) (elem[0].i64);
       elem++;
       switch (typeid) {
           case DriftID:
                Drift_track(p, (__constant Drift*) elem);
           break;
           case DriftExactID:
                DriftExact_track(p, (__constant DriftExact*) elem);
           break;
           case MultipoleID:
                Multipole_track(p, (__constant Multipole*) elem);
           break;
           case CavityID:
                Cavity_track(p, (__constant Cavity*) elem);
           break;
           case AlignID:
                Align_track(p, (__constant Align*) elem);
           break;
           case LinMapID:
                LinMap_track(p, (__constant LinMap_data*) elem);
           break;
           case BB4DID:
                BB4D_track(p, (__constant BB4D_data *) elem);
           break;
           case BlockID: break;
       }
   //}
   return 1;
}


// Tracking loop

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
) {
   { // This is in a block because i_part isn't required outside so doesn't need function-level lifetime.
     unsigned  i_part = get_global_id(0);
     // Since each worker only works on a single particle,
     // shift all the particle-related pointers to point at the assigned particle.
     particles += i_part;
     #ifdef TRACK_BY_TURN
       particles_by_turn += i_part;
     #endif
     #ifdef TRACK_BY_ELEMENT
       particles_by_element += i_part;
     #endif
   }
   unsigned int i_elem, c_elem;
   unsigned int nelem = elements_offsets[0];
   while(nturn--) {
     i_elem = 1;
     for(c_elem = nelem; c_elem--; i_elem++) {
       track_single(elements_data+elements_offsets[i_elem], particles);
       #ifdef TRACK_BY_ELEMENT
         *particles_by_element = *particles;
         particles_by_element += npart;
       #endif
     }
     particles->turn += select(0, 1, particles->state >= 0);
     #ifdef TRACK_BY_TURN
       *particles_by_turn = *particles;
       particles_by_turn += npart;
     #endif
   }
}

#else
#include <math.h>

void Block_track(value_t *elements_data, unsigned int *elements_offsets, tracking_beam *beam,
                unsigned int nturn, unsigned int npart,
                Particle *particles_by_turn, Particle *particles_by_element){
   while(nturn--) {
     for(unsigned int i_elem = 1; i_elem <= elements_offsets[0]; i_elem++) {
       for(unsigned int i_part = 0; i_part < npart; i_part++) {
          if(beam->b_particles[i_part].state >= 0) {
            track_single(elements_data+elements_offsets[i_elem], (beam->b_particles)+i_part);
            if(particles_by_element != NULL) {
              particles_by_element[i_part] = beam->b_particles[i_part];
            }
            if(particles_by_turn != NULL && i_elem == elements_offsets[0]) {
              particles_by_turn[i_part] = beam->b_particles[i_part];
            }
          }
       }
       if(particles_by_element != NULL) {
         particles_by_element += npart;
       }
     }
     for(unsigned int i_part=0; i_part < npart; i_part++) {
       if (beam->b_particles[i_part].state >= 0) {
         beam->b_particles[i_part].turn++;
       }
     }
     if(particles_by_turn != NULL) {
       particles_by_turn += npart;
     }
   }
}

#endif


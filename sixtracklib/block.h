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

enum element_type {
	DriftID, DriftExactID,
	MultipoleID, CavityID, AlignID,
	LinMapID, BB4DID,
	BlockID
};

#ifdef _GPUCODE
#include "../common/particle_v.h"

CLKERNEL void Block_track(__constant value_t * elements_data,
			  __constant unsigned int *elements_offsets,
			  CLGLOBAL struct particle_v *particles,
			  unsigned int nturn, unsigned int npart
#ifdef TRACK_BY_TURN
			  , CLGLOBAL struct particle_v *particles_by_turn
#endif
#ifdef TRACK_BY_ELEMENT
			  , CLGLOBAL struct particle_v *particles_by_element
#endif
    );

#else

/*void Block_track(value_t * elements_data, unsigned int *elements_offsets,
		 struct tracking_beam *beam,
		 unsigned int nturn, unsigned int npart,
		 struct particle *particles_by_turn,
		 struct particle *particles_by_element);*/

#endif

#endif

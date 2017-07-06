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

#ifdef _GPUCODE
#include "../common/track.h"


_CUDA_HOST_DEVICE_
void track_single(__constant value_t * elem, CLGLOBAL struct particle_v *p)
{
	enum element_type typeid = (enum element_type)(elem[0].i64);
	elem++;
	switch (typeid) {
	case DriftID:
		drift_track(p, (__constant struct drift *)elem);
		break;
	case DriftExactID:
		drift_exact_track(p, (__constant struct drift_exact *)elem);
		break;
	case MultipoleID:
		multipole_track(p, (__constant struct multipole *)elem);
		break;
	case CavityID:
		cavity_track(p, (__constant struct cavity *)elem);
		break;
	case AlignID:
		align_track(p, (__constant struct align *)elem);
		break;
	case LinMapID:
		linmap_track(p, (__constant struct linmap_data *)elem);
		break;
	case BB4DID:
		bb4d_track(p, (__constant struct bb4d_data *)elem);
		break;
	case BlockID:
		break;
	}
}

// Tracking kernel

//#ifdef _GPUCODE

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
			  )
{
	{			// This is in a block because i_part isn't required outside so doesn't need function-level lifetime.
		unsigned i_part = get_global_id(0);
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
	while (nturn--) {
		i_elem = 1;
		for (c_elem = nelem; c_elem--; i_elem++) {
			track_single(elements_data + elements_offsets[i_elem],
				     (CLGLOBAL struct particle_v *)particles);
#ifdef TRACK_BY_ELEMENT
			*particles_by_element = *particles;
			particles_by_element += npart;
#endif
		}
		particles->turn += select((int2)(0, 0), (int2)(1, 1), particles->state >= 0);
#ifdef TRACK_BY_TURN
		*particles_by_turn = *particles;
		particles_by_turn += npart;
#endif
	}
}

#else

#include <math.h>
/*
void Block_track(value_t * elements_data, unsigned int *elements_offsets,
		 struct tracking_beam *beam,
		 unsigned int nturn, unsigned int npart,
		 struct particle *particles_by_turn,
		 struct particle *particles_by_element)
{
	while (nturn--) {
		for (unsigned int i_elem = 1; i_elem <= elements_offsets[0]; i_elem++) {
			for (unsigned int i_part = 0; i_part < npart; i_part++) {
				if (beam->b_particles[i_part].state >= 0) {
					track_single(elements_data + elements_offsets[i_elem],
						     (beam->b_particles) + i_part);
					if (particles_by_element != NULL) {
						particles_by_element[i_part] = beam->b_particles[i_part];
					}
					if (particles_by_turn != NULL
					    && i_elem == elements_offsets[0]) {
						particles_by_turn[i_part] = beam->b_particles[i_part];
					}
				}
			}
			if (particles_by_element != NULL) {
				particles_by_element += npart;
			}
		}
		for (unsigned int i_part = 0; i_part < npart; i_part++) {
			if (beam->b_particles[i_part].state >= 0) {
				beam->b_particles[i_part].turn++;
			}
		}
		if (particles_by_turn != NULL) {
			particles_by_turn += npart;
		}
	}
}
*/
#endif

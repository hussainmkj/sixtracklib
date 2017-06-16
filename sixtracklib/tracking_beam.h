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



#ifndef _TRACKING_BEAM_
#define _TRACKING_BEAM_

#include "particle.h"

typedef struct tracking_beam {
  unsigned long b_npart;
  Particle* b_particles;
} tracking_beam;

tracking_beam* tracking_beam_create_args(unsigned int n, double mass0, double p0c);
tracking_beam* tracking_beam_create(unsigned int n);
void tracking_beam_clean(tracking_beam *beam);

#endif

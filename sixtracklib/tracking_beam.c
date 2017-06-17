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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tracking_beam.h"
#include "tracking_err.h"

#define clight 299792458
#define pi 3.141592653589793238
#define pcharge 1.602176565e-19
#define echarge (-pcharge)
#define emass 0.510998928e6
#define pmass 938.272046e6
#define epsilon0 8.854187817e-12
#define mu0 (4e-7*pi)
#define eradius ((pcharge*pcharge)/(4*pi*epsilon0*emass*clight*clight))
#define pradius (pcharge**2/(4*pi*epsilon0*pmass*clight**2))
#define anumber 6.022140857e23
#define kboltz 8.6173303e-5

struct tracking_beam *tracking_beam_create_args(unsigned int n, double mass0,
						double p0c)
{
	struct tracking_beam *beam =
	    (struct tracking_beam *)malloc(sizeof(struct tracking_beam));
	if (beam == NULL) {
		perror("Failed to allocate tracking beam structure.");
		return NULL;
	}
	struct particle *particles =
	    (struct particle *)malloc(n * sizeof(struct particle));
	if (particles == NULL) {
		perror("Failed to allocate particles.");
		return NULL;
	}
	beam->b_npart = n;
	beam->b_particles = particles;
	struct particle *p = particles;
	p->partid = 0;
	p->elemid = 0;
	p->turn = 0;
	p->state = 0;
	p->s = 0.0;
	p->x = 0.0;
	p->px = 0.0;
	p->y = 0.0;
	p->py = 0.0;
	p->sigma = 0.0;
	p->psigma = 0.0;
	p->delta = 0.0;
	p->beta = 0.0;
	p->gamma = 0.0;
	p->q0 = 0.0;
	p->q = 0.0;
	p->m0 = mass0;
	const double energy0 = sqrt(p0c * p0c + mass0 * mass0);
	const double gamma0 = energy0 / mass0;
	const double beta0 = p0c / mass0 / gamma0;
	const double chi = 1.0;
	p->partid = 0;
	p->chi = chi;
	p->beta0 = beta0;
	p->gamma0 = gamma0;
	p->p0c = p0c;
	p->rvv = 1.0;
	p->rpp = 1.0;
	for (unsigned int i = 1; i < n; i++) {
		particles[i] = *p;
		particles[i].partid = i;
	}
	return beam;
}

struct tracking_beam *tracking_beam_create(unsigned int n)
{
	return tracking_beam_create_args(n, pmass, 450.0);
}

void tracking_beam_clean(struct tracking_beam *beam)
{
	free(beam->b_particles);
	free(beam);
}

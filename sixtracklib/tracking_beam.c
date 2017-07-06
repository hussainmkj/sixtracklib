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

void tracking_beam_pack(struct tracking_beam *src_beam, struct particle_2 *dest)
{
	int p_i, f_i;
	int n = src_beam->b_npart;
	struct particle *src = src_beam->b_particles;
	for (int i = 0; i < n; i++) {
		p_i = i/2;
		f_i = i%2;
		dest[p_i].partid[f_i] = src[i].partid;
		dest[p_i].elemid[f_i] = src[i].elemid;
		dest[p_i].turn[f_i] = src[i].turn;
		dest[p_i].state[f_i] = src[i].state;
		dest[p_i].s[f_i] = src[i].s;
		dest[p_i].x[f_i] = src[i].x;
		dest[p_i].px[f_i] = src[i].px;
		dest[p_i].y[f_i] = src[i].y;
		dest[p_i].py[f_i] = src[i].py;
		dest[p_i].sigma[f_i] = src[i].sigma;
		dest[p_i].psigma[f_i] = src[i].psigma;
		dest[p_i].chi[f_i] = src[i].chi;
		dest[p_i].delta[f_i] = src[i].delta;
		dest[p_i].rpp[f_i] = src[i].rpp;
		dest[p_i].rvv[f_i] = src[i].rvv;
		dest[p_i].beta[f_i] = src[i].beta;
		dest[p_i].gamma[f_i] = src[i].gamma;
		dest[p_i].m0[f_i] = src[i].m0;
		dest[p_i].q0[f_i] = src[i].q0;
		dest[p_i].q[f_i] = src[i].q;
		dest[p_i].beta0[f_i] = src[i].beta0;
		dest[p_i].gamma0[f_i] = src[i].gamma0;
		dest[p_i].p0c[f_i] = src[i].p0c;
	}
}

void tracking_beam_unpack(struct particle_2 *src, struct tracking_beam *dest_beam)
{
	int n = dest_beam->b_npart;
	n = (n/2)+(n%2);
	struct particle *dest = dest_beam->b_particles;
	for (int p_i = 0; p_i < n; p_i++) {
		for (int f_i = 0; f_i < 2; f_i++) {
			dest[p_i+f_i].partid = src[p_i].partid[f_i];
			dest[p_i+f_i].elemid = src[p_i].elemid[f_i];
			dest[p_i+f_i].turn = src[p_i].turn[f_i];
			dest[p_i+f_i].state = src[p_i].state[f_i];
			dest[p_i+f_i].s = src[p_i].s[f_i];
			dest[p_i+f_i].x = src[p_i].x[f_i];
			dest[p_i+f_i].px = src[p_i].px[f_i];
			dest[p_i+f_i].y = src[p_i].y[f_i];
			dest[p_i+f_i].py = src[p_i].py[f_i];
			dest[p_i+f_i].sigma = src[p_i].sigma[f_i];
			dest[p_i+f_i].psigma = src[p_i].psigma[f_i];
			dest[p_i+f_i].chi = src[p_i].chi[f_i];
			dest[p_i+f_i].delta = src[p_i].delta[f_i];
			dest[p_i+f_i].rpp = src[p_i].rpp[f_i];
			dest[p_i+f_i].rvv = src[p_i].rvv[f_i];
			dest[p_i+f_i].beta = src[p_i].beta[f_i];
			dest[p_i+f_i].gamma = src[p_i].gamma[f_i];
			dest[p_i+f_i].m0 = src[p_i].m0[f_i];
			dest[p_i+f_i].q0 = src[p_i].q0[f_i];
			dest[p_i+f_i].q = src[p_i].q[f_i];
			dest[p_i+f_i].beta0 = src[p_i].beta0[f_i];
			dest[p_i+f_i].gamma0 = src[p_i].gamma0[f_i];
			dest[p_i+f_i].p0c = src[p_i].p0c[f_i];
		}
	}
}

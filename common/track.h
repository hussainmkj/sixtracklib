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

#ifndef _TRACK_
#define _TRACK_

#ifndef _GPUCODE
#include <math.h>
#include <stdio.h>
#define CLGLOBAL
#endif

#include "particle.h"
#include "particle_v.h"
#include "constants.h"

/******************************************/
/*Drift*/
/******************************************/

struct drift {
	double length;
};

_CUDA_HOST_DEVICE_
void drift_track(CLGLOBAL struct particle_v *p, __constant struct drift *el)
{
	doublePSIZE xp, yp;
	double length = el->length;
	xp = p->px * p->rpp;
	yp = p->py * p->rpp;
	p->x += xp * length;
	p->y += yp * length;
	p->sigma += length * (1 - p->rvv * (1 + (xp * xp + yp * yp) / 2));
	p->s += length;
}

/******************************************/
/*Exact Drift*/
/******************************************/

struct drift_exact {
	double length;
};

_CUDA_HOST_DEVICE_
void drift_exact_track(CLGLOBAL struct particle_v *p,
			__constant struct drift_exact *el)
{
	doublePSIZE lpzi, lbzi, px, py, opd;
	double length = el->length;
	opd = 1 + p->delta;
	px = p->px;
	py = p->py;
	lpzi = length / sqrt(opd * opd - px * px - py * py);
	lbzi = (p->beta0 * p->beta0 * p->psigma + 1) * lpzi;
	p->x += px * lpzi;
	p->y += py * lpzi;
	p->sigma += length - lbzi;
	p->s += length;
}

/******************************************/
/*Multipole*/
/******************************************/

struct multipole {
	long int order;
	double l;
	double hxl;
	double hyl;
	double bal[1];
};

_CUDA_HOST_DEVICE_
void multipole_track(CLGLOBAL struct particle_v *p,
		     __constant struct multipole *el)
{
	doublePSIZE x, y, chi, dpx, dpy, zre, zim, b1l, a1l, hxx, hyy;
	long int order = el->order;
	double hxl = el->hxl;
	double hyl = el->hyl;
	double l = el->l;
	__constant double *bal = el->bal;
	dpx = bal[order * 2];
	dpy = bal[order * 2 + 1];
	x = p->x;
	y = p->y;
	chi = p->chi;
	for (int ii = order - 1; ii >= 0; ii--) {
		zre = (dpx * x - dpy * y);
		zim = (dpx * y + dpy * x);
		dpx = bal[ii * 2] + zre;
		dpy = bal[ii * 2 + 1] + zim;
	}
	dpx = -chi * dpx;
	dpy = chi * dpy;
	if (l > 0) {
		b1l = chi * bal[0];
		a1l = chi * bal[1];
		hxx = hxl / l * x;
		hyy = hyl / l * y;
		dpx += hxl + hxl * p->delta - b1l * hxx;
		dpy -= hyl + hyl * p->delta - a1l * hyy;
		p->sigma -= chi * (hxx - hyy) * l * p->rvv;
	}
	p->px += dpx;
	p->py += dpy;
}

/******************************************/
/*RF Cavity*/
/******************************************/

struct cavity {
	double volt;
	double freq;
	double lag;
};

_CUDA_HOST_DEVICE_
void cavity_track(CLGLOBAL struct particle_v *p, __constant struct cavity *el)
{
	double volt = el->volt;
	double freq = el->freq;
	double lag = el->lag;
	doublePSIZE phase, pt, opd;
	phase = lag - 2 * M_PI / C_LIGHT * freq * p->sigma / p->beta0;
	p->psigma += p->chi * volt * sin(phase) / (p->p0c * p->beta0);
	pt = p->psigma * p->beta0;
	opd = sqrt(pt * pt + 2 * p->psigma + 1);
	p->delta = opd - 1;
	p->beta = opd / (1 / p->beta0 + pt);
	p->gamma = (pt * p->beta0 + 1) * p->gamma0;
	p->rpp = 1 / opd;
	p->rvv = p->beta0 / p->beta;
}

/******************************************/
/*Align*/
/******************************************/

struct align {
	double cz;
	double sz;
	double dx;
	double dy;
};

_CUDA_HOST_DEVICE_
void align_track(CLGLOBAL struct particle_v *p, __constant struct align *el)
{
	doublePSIZE xn, yn;
	double cz = el->cz;
	double sz = el->sz;
	double dx = el->dx;
	double dy = el->dy;
	xn = cz * p->x - sz * p->y - dx;
	yn = sz * p->x + cz * p->y - dy;
	p->x = xn;
	p->y = yn;
	xn = cz * p->px + sz * p->py;
	yn = -sz * p->px + cz * p->py;
	p->px = xn;
	p->py = yn;
}

/******************************************/
/*Linear Map*/
/******************************************/

struct linmap_data {
	double matrix[8];
	//double disp[4];
};

_CUDA_HOST_DEVICE_
struct linmap_data linmap_init(double alpha_x_s0, double beta_x_s0,
				double alpha_x_s1, double beta_x_s1,
				double alpha_y_s0, double beta_y_s0,
				double alpha_y_s1, double beta_y_s1,
				double dQ_x, double dQ_y)
{
	struct linmap_data res;
	double s, c;

	//sincos(dQ_x, &s, &c);
	s = sin(2. * M_PI * dQ_x);
	c = cos(2. * M_PI * dQ_x);
	res.matrix[0] = sqrt(beta_x_s1 / beta_x_s0) * (c + alpha_x_s0 * s);
	res.matrix[1] = sqrt(beta_x_s1 * beta_x_s0) * s;
	res.matrix[2] =
	    ((alpha_x_s0 - alpha_x_s1) * c -
	     (1. + alpha_x_s0 * alpha_x_s1) * s) / sqrt(beta_x_s1 * beta_x_s0);
	res.matrix[3] = sqrt(beta_x_s0 / beta_x_s1) * (c - alpha_x_s1 * s);

	//sincos(dQ_y, &s, &c);
	s = sin(dQ_y);
	c = cos(dQ_y);
	res.matrix[4] = sqrt(beta_y_s1 / beta_y_s0) * (c + alpha_y_s0 * s);
	res.matrix[5] = sqrt(beta_y_s1 * beta_y_s0) * s;
	res.matrix[6] =
	    ((alpha_y_s0 - alpha_y_s1) * c -
	     (1. + alpha_y_s0 * alpha_y_s1) * s) / sqrt(beta_y_s1 * beta_y_s0);
	res.matrix[7] = sqrt(beta_y_s0 / beta_y_s1) * (c - alpha_y_s1 * s);
	return res;
}

_CUDA_HOST_DEVICE_
void linmap_track(CLGLOBAL struct particle_v *p,
		  __constant struct linmap_data *el)
{
	double M00 = el->matrix[0];
	double M01 = el->matrix[1];
	double M10 = el->matrix[2];
	double M11 = el->matrix[3];
	double M22 = el->matrix[4];
	double M23 = el->matrix[5];
	double M32 = el->matrix[6];
	double M33 = el->matrix[7];
	doublePSIZE x0 = p->x;
	doublePSIZE px0 = p->px;
	doublePSIZE y0 = p->y;
	doublePSIZE py0 = p->py;

	p->x = M00 * x0 + M01 * px0;
	p->px = M10 * x0 + M11 * px0;
	p->y = M22 * y0 + M23 * py0;
	p->py = M32 * y0 + M33 * py0;
}

/******************************************/
/*Beam-beam 4d*/
/******************************************/

#include "transverse_field_gauss_round.h"
#include "transverse_field_gauss_ellip.h"

struct bb4d_data {
	double N_s;		// Population strong beam
	double beta_s;
	double q_s;
	long int trasv_field_type;	//1: round gaussian
	__constant void *field_map_data;
};

_CUDA_HOST_DEVICE_
void bb4d_track(CLGLOBAL struct particle_v *p,
		__constant struct bb4d_data *el)
{
	doublePSIZE Ex, Ey;

#ifdef DATA_PTR_IS_OFFSET
	__constant void *ptr =
	    ((__constant uint64_t *) (&(el->field_map_data))) +
	    ((uint64_t) el->field_map_data) + 1;
#else
	void *ptr = el->field_map_data;
#endif

	switch (el->trasv_field_type) {
	case 1:
		get_transv_field_gauss_round((__constant struct
					      transv_field_gauss_round_data *)
					     ptr, p->x, p->y, &Ex, &Ey);
		break;
	case 2:
		get_transv_field_gauss_ellip((__constant struct
					      transv_field_gauss_ellip_data *)
					     ptr, p->x, p->y, &Ex, &Ey);
		break;
	default:
		Ex = 1 / 0.;
		Ey = 1 / 0.;
	}

	doublePSIZE fact_kick =
	    p->chi * el->N_s * el->q_s * p->q0 * (1. +
						  p->beta * el->beta_s) /
	    (p->p0c * QELEM * (p->beta + el->beta_s));

	p->px += fact_kick * Ex;
	p->py += fact_kick * Ey;
}

#endif

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

#ifndef _PARTICLE_
#define _PARTICLE_

#define PSIZE 2
#define doublePSIZE double2
#define intPSIZE int2

struct particle {
	int partid;
	int elemid;
	int turn;
	int state;		//negative particle lost
	double s;
	double x;
	double px;		// Px/P0
	double y;
	double py;		// Px/P0
	double sigma;
	double psigma;		// (E-E0)/ (beta0 P0c)
	double chi;		// q/q0 * m/m0
	double delta;
	double rpp;		// ratio P0/P
	double rvv;		// ratio beta / beta0
	double beta;
	double gamma;
	double m0;		// eV
	double q0;		// C
	double q;		// C
	double beta0;
	double gamma0;
	double p0c;		//eV
};

struct particle_2 {
	int partid[PSIZE];
	int elemid[PSIZE];
	int turn[PSIZE];
	int state[PSIZE];		//negative particle lost
	double s[PSIZE];
	double x[PSIZE];
	double px[PSIZE];		// Px/P0
	double y[PSIZE];
	double py[PSIZE];		// Px/P0
	double sigma[PSIZE];
	double psigma[PSIZE];		// (E-E0)/ (beta0 P0c)
	double chi[PSIZE];		// q/q0 * m/m0
	double delta[PSIZE];
	double rpp[PSIZE];		// ratio P0/P
	double rvv[PSIZE];		// ratio beta / beta0
	double beta[PSIZE];
	double gamma[PSIZE];
	double m0[PSIZE];		// eV
	double q0[PSIZE];		// C
	double q[PSIZE];		// C
	double beta0[PSIZE];
	double gamma0[PSIZE];
	double p0c[PSIZE];		//eV
};

#endif

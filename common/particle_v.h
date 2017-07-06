#ifndef _PARTICLE_V_
#define _PARTICLE_V_

struct particle_v {
	intPSIZE partid;
	intPSIZE elemid;
	intPSIZE turn;
	intPSIZE state;		//negative particle lost
	doublePSIZE s;
	doublePSIZE x;
	doublePSIZE px;		// Px/P0
	doublePSIZE y;
	doublePSIZE py;		// Px/P0
	doublePSIZE sigma;
	doublePSIZE psigma;		// (E-E0)/ (beta0 P0c)
	doublePSIZE chi;		// q/q0 * m/m0
	doublePSIZE delta;
	doublePSIZE rpp;		// ratio P0/P
	doublePSIZE rvv;		// ratio beta / beta0
	doublePSIZE beta;
	doublePSIZE gamma;
	doublePSIZE m0;		// eV
	doublePSIZE q0;		// C
	doublePSIZE q;		// C
	doublePSIZE beta0;
	doublePSIZE gamma0;
	doublePSIZE p0c;		//eV
};
#endif

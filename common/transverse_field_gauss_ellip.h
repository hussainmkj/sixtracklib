#ifndef _TRANS_FIELD_GUASS_ELLIP_
#define _TRANS_FIELD_GUASS_ELLIP_

#include "constants.h"
#include "cmpx.h"

#ifndef _GPUCODE
#include <math.h>
#define CLGLOBAL
#endif

// To use MIT Faddeeva library uncomment following
//#include <complex.h>
//#include "Faddeeva.h" 
//cmpx wfun(cmpx zz){
//    double complex temp;
//    cmpx res;
//    temp = Faddeeva_w((zz.r + I*zz.i), 0.);
//    res = makecmpx(creal(temp), cimag(temp));
//    return res;
//}   

// To use CERNLIB Faddeeva 
#include "faddeeva_cern.h"

_CUDA_HOST_DEVICE_ cmpx2 wfun(cmpx2 zz)
{
	cmpx2 res;
	cmpx res1, res2;
	cerrf(zz.r.x, zz.i.x, &(res1.r), &(res2.i));
	cerrf(zz.r.y, zz.i.y, &(res2.r), &(res2.i));
	res.r = (double2)(res1.r, res2.r);
	res.i = (double2)(res1.i, res2.i);
	return res;
}

struct transv_field_gauss_ellip_data {
	double sigma_x;
	double sigma_y;
	double Delta_x;
	double Delta_y;
};

_CUDA_HOST_DEVICE_
void get_transv_field_gauss_ellip(__constant struct
				  transv_field_gauss_ellip_data *data,
				  doublePSIZE x, doublePSIZE y, doublePSIZE *Ex_out,
				  doublePSIZE *Ey_out)
{
	double sigmax = data->sigma_x;
	double sigmay = data->sigma_y;

	// I always go to the first quadrant and then apply the signs a posteriori
	// numerically more stable (see http://inspirehep.net/record/316705/files/slac-pub-5582.pdf)

	doublePSIZE abx = fabs(x - data->Delta_x);
	doublePSIZE aby = fabs(y - data->Delta_y);

	//printf("x = %.2e y = %.2e abx = %.2e aby = %.2e", xx, yy, abx, aby);

	double S, factBE;
double2 Ex, Ey;
	cmpx2 etaBE, zetaBE, val;

	if (sigmax > sigmay) {
		S = sqrt(2. * (sigmax * sigmax - sigmay * sigmay));
		factBE = 1. / (2. * EPSILON_0 * SQRT_PI * S);

		etaBE = makecmpx2(sigmay / sigmax * abx, sigmax / sigmay * aby);
		zetaBE = makecmpx2(abx, aby);

		val = csub2(wfun(cscale2(zetaBE, 1. / S)),
			   cscale2(wfun(cscale2(etaBE, 1. / S)),
				  exp(-abx * abx / (2 * sigmax * sigmax) -
				      aby * aby / (2 * sigmay * sigmay))));

		Ex = factBE * val.i;
		Ey = factBE * val.r;
	} else if (sigmax < sigmay) {
		S = sqrt(2. * (sigmay * sigmay - sigmax * sigmax));
		factBE = 1. / (2. * EPSILON_0 * SQRT_PI * S);

		etaBE = makecmpx2(sigmax / sigmay * aby, sigmay / sigmax * abx);
		zetaBE = makecmpx2(aby, abx);

		val = csub2(wfun(cscale2(zetaBE, 1. / S)),
			   cscale2(wfun(cscale2(etaBE, 1. / S)),
				  exp(-aby * aby / (2 * sigmay * sigmay) -
				      abx * abx / (2 * sigmax * sigmax))));

		Ey = factBE * val.i;
		Ex = factBE * val.r;
	} else {
		//printf("Round beam not implemented!\n");
		//exit(1);
		Ex = Ey = 1. / 0.;
	}

	if (all((x - data->Delta_x) < 0))
		Ex = -Ex;
	if (all((y - data->Delta_y) < 0))
		Ey = -Ey;

	(*Ex_out) = Ex;
	(*Ey_out) = Ey;
}

#endif

#ifndef _MY_CMPX_
#define _MY_CMPX_

#ifndef _GPUCODE
#include <math.h>
#endif

typedef struct {
	double r;
	double i;
} cmpx;

typedef struct {
	double2 r;
	double2 i;
} cmpx2;


_CUDA_HOST_DEVICE_ cmpx makecmpx(double r, double i)
{
	cmpx res;
	res.r = r;
	res.i = i;
	return res;
}

_CUDA_HOST_DEVICE_ cmpx2 makecmpx2(double2 r, double2 i)
{
	cmpx2 res;
	res.r = r;
	res.i = i;
	return res;
}


_CUDA_HOST_DEVICE_ cmpx cadd(cmpx a, cmpx b)
{
	a.r += b.r;
	a.i += b.i;
	return a;
}

_CUDA_HOST_DEVICE_ cmpx2 cadd2(cmpx2 a, cmpx2 b)
{
	a.r += b.r;
	a.i += b.i;
	return a;
}


_CUDA_HOST_DEVICE_ cmpx csub(cmpx a, cmpx b)
{
	a.r -= b.r;
	a.i -= b.i;
	return a;
}

_CUDA_HOST_DEVICE_ cmpx2 csub2(cmpx2 a, cmpx2 b)
{
	a.r -= b.r;
	a.i -= b.i;
	return a;
}


_CUDA_HOST_DEVICE_ cmpx cmul(cmpx a, cmpx b)
{
	cmpx res;
	res.r = a.r * b.r - a.i * b.i;
	res.i = a.i * b.r + a.r * b.i;
	return res;
}

_CUDA_HOST_DEVICE_ cmpx2 cmul2(cmpx2 a, cmpx2 b)
{
	cmpx2 res;
	res.r = a.r * b.r - a.i * b.i;
	res.i = a.i * b.r + a.r * b.i;
	return res;
}


_CUDA_HOST_DEVICE_ cmpx cdiv(cmpx a, cmpx b)
{
	double den;
	cmpx res;
	den = b.r * b.r + b.i * b.i;
	res.r = (a.r * b.r + a.i * b.i) / den;
	res.i = (a.i * b.r - a.r * b.i) / den;
	return res;
}

_CUDA_HOST_DEVICE_ cmpx2 cdiv2(cmpx2 a, cmpx2 b)
{
	double2 den;
	cmpx2 res;
	den = b.r * b.r + b.i * b.i;
	res.r = (a.r * b.r + a.i * b.i) / den;
	res.i = (a.i * b.r - a.r * b.i) / den;
	return res;
}

_CUDA_HOST_DEVICE_ cmpx expc(cmpx a)
{
	double mod;
	cmpx res;
	mod = exp(a.r);
	res.r = mod * cos(a.i);
	res.i = mod * sin(a.i);
	return res;
}

_CUDA_HOST_DEVICE_ cmpx2 expc2(cmpx2 a)
{
	double2 mod;
	cmpx2 res;
	mod = exp(a.r);
	res.r = mod * cos(a.i);
	res.i = mod * sin(a.i);
	return res;
}

_CUDA_HOST_DEVICE_ cmpx cscale(cmpx a, double f)
{
	a.r *= f;
	a.i *= f;
	return a;
}

_CUDA_HOST_DEVICE_ cmpx2 cscale2(cmpx2 a, double2 f)
{
	a.r *= f;
	a.i *= f;
	return a;
}

#endif

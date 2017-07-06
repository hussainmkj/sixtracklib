#include <stdio.h>
#include <assert.h>
#include <sys/time.h>
#include "tracking_cl.h"

int main(int argc, char **argv)
{
	if(argc != 3) {
		return 1;
	}
	int npart = atoi(argv[1]);
	int nturn = atoi(argv[2]);
	struct timeval tv1, tv2;
	struct tracking_elements *elements = tracking_elements_create(2);
	double knl[3] = { 1.0, 3.0, 5.0 };
	double ksl[3] = { 2.0, 4.0, 6.0 };
	tracking_elements_add_multipole(elements, knl, 3, ksl, 3, 0, 0, 0);
	tracking_elements_add_drift(elements, 56.0);
	tracking_elements_add_drift(elements, 5.0);
	struct tracking_beam *beam = tracking_beam_create(npart);
	struct tracking_cl *track =
	    tracking_cl_prepare(elements, beam, nturn, 1, 1);
	gettimeofday(&tv1, NULL);
	tracking_cl_execute(track);
	clFinish(track->t_queue);
	gettimeofday(&tv2, NULL);
	tracking_cl_read(track);
	tracking_cl_clean(track);
	double perf =
	    (double)(tv2.tv_usec - tv1.tv_usec) / 1000000 +
	    (double)(tv2.tv_sec - tv1.tv_sec);
	printf("%F\n", perf);
	return 0;
}

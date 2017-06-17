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

#ifndef _TRACKING_ELEMENTS_
#define _TRACKING_ELEMENTS_

#include "value.h"

struct tracking_elements {
	unsigned int te_size;
	unsigned int te_head;
	value_t *te_data;
	unsigned int *te_offsets;
};

struct tracking_elements *tracking_elements_create(unsigned int size);
void tracking_elements_reshape(struct tracking_elements *elements,
			       unsigned int n);
void tracking_elements_clean(struct tracking_elements *elements);
void tracking_elements_allocate_element(struct tracking_elements *elements,
					unsigned int n);
void tracking_elements_add_datum_f64(struct tracking_elements *elements,
				     double datum);
void tracking_elements_add_datum_u64(struct tracking_elements *elements,
				     unsigned long datum);
void tracking_elements_add_drift(struct tracking_elements *elements,
				 double length);
void tracking_elements_add_cavity(struct tracking_elements *elements,
				  double volt, double freq, double lag);
void tracking_elements_add_align(struct tracking_elements *elements,
				 double tilt, double dx, double dy);
void tracking_elements_add_multipole(struct tracking_elements *elements,
				     double *knl, unsigned int knl_len,
				     double *ksl, unsigned int ksl_len,
				     double length, double hxl, double hyl);

#endif

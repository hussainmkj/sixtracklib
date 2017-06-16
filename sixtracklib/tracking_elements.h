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

typedef struct {
    unsigned int te_size;
    unsigned int te_head;
    value_t *te_data;
    unsigned int *te_offsets;
} tracking_elements;

tracking_elements* tracking_elements_create(unsigned int size);
void tracking_elements_reshape(tracking_elements *elements, unsigned int n);
void tracking_elements_clean(tracking_elements *elements);
void tracking_elements_allocate_element(tracking_elements *elements, unsigned int n);
void tracking_elements_add_datum_f64(tracking_elements *elements, double datum);
void tracking_elements_add_datum_u64(tracking_elements *elements, unsigned long datum);
void tracking_elements_add_drift(tracking_elements *elements, double length);
void tracking_elements_add_cavity(tracking_elements *elements, double volt, double freq, double lag);
void tracking_elements_add_align(tracking_elements *elements, double tilt, double dx, double dy);
void tracking_elements_add_multipole(tracking_elements *elements, double *knl, unsigned int knl_len, double *ksl, unsigned int ksl_len, double length, double hxl, double hyl);

#endif

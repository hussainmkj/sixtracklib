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
#include "block.h"
#include "constants.h"
#include "tracking_elements.h"
#include "tracking_err.h"

tracking_elements* tracking_elements_create(unsigned int size) {
    tracking_elements *elements = (tracking_elements *)malloc(sizeof(tracking_elements));
    if(elements == NULL) {
        perror("Failed to allocate tracking_cl elements structure");
        return NULL;
    }
    if(!size) {
        size = 512;
    }
    value_t *data = (value_t *)malloc(sizeof(value_t) * size);
    if(data == NULL) {
        perror("Failed to allocate tracking_cl elements data");
        return NULL;
    }
    unsigned int *offsets = (unsigned int *)malloc(sizeof(unsigned int)*(size+1));
    if(offsets == NULL) {
        perror("Failed to allocate tracking_cl elements offsets");
        return NULL;
    }
    offsets[0] = 0;
    elements->te_size = size;
    elements->te_head = 0;
    elements->te_data = data;
    elements->te_offsets = offsets;
    return elements;
}

void tracking_elements_reshape(tracking_elements *elements, unsigned int n) {
    if(elements->te_head+n < elements->te_size) {
        return;
    }
    int nsize = (elements->te_size+n)*2;
    value_t *ndata = (value_t *)realloc(elements->te_data, sizeof(value_t)*nsize);
    if(ndata == NULL) {
        perror("Failed to reallocate tracking_cl elements data.");
        return;
    }
    unsigned int *noffsets = (unsigned int *)realloc(elements->te_offsets, sizeof(unsigned int)*(nsize+1));
    if(noffsets == NULL) {
        perror("Failed to reallocate tracking_cl elements offsets.");
        return;
    }
    elements->te_size = nsize;
    elements->te_data = ndata;
    elements->te_offsets = noffsets;
}

void tracking_elements_clean(tracking_elements *elements) {
    free(elements->te_data);
    free(elements->te_offsets);
    free(elements);
}

void tracking_elements_allocate_element(tracking_elements *elements, unsigned int n) {
    tracking_elements_reshape(elements, n);
    elements->te_offsets[0]++;
    elements->te_offsets[elements->te_offsets[0]] = elements->te_head;
}

void tracking_elements_add_datum_f64(tracking_elements *elements, double datum) {
    elements->te_data[elements->te_head++].f64 = datum;
}

void tracking_elements_add_datum_u64(tracking_elements *elements, unsigned long datum) {
    elements->te_data[elements->te_head++].u64 = datum;
}


void tracking_elements_add_drift(tracking_elements *elements, double length) {
    tracking_elements_allocate_element(elements, 2);
    tracking_elements_add_datum_u64(elements, DriftID);
    tracking_elements_add_datum_f64(elements, length);
}

void tracking_elements_add_cavity(tracking_elements *elements, double volt, double freq, double lag) {
    tracking_elements_allocate_element(elements, 4);
    tracking_elements_add_datum_u64(elements, CavityID);
    tracking_elements_add_datum_f64(elements, volt);
    tracking_elements_add_datum_f64(elements, freq);
    tracking_elements_add_datum_f64(elements, lag/180.0 * M_PI);
}

void tracking_elements_add_align(tracking_elements *elements, double tilt, double dx, double dy) {
    tracking_elements_allocate_element(elements, 5);
    tracking_elements_add_datum_u64(elements, AlignID);
    tracking_elements_add_datum_f64(elements, cos(tilt/180.0 * M_PI));
    tracking_elements_add_datum_f64(elements, sin(tilt/180.0 * M_PI));
    tracking_elements_add_datum_f64(elements, dx);
    tracking_elements_add_datum_f64(elements, dy);
}

void tracking_elements_add_multipole(tracking_elements *elements, double *knl, unsigned int knl_len, double *ksl, unsigned int ksl_len, double length, double hxl, double hyl) {
    double bal[(knl_len >= ksl_len ? 2*knl_len : 2*ksl_len)];
    unsigned int i = 0;
    for(; i < knl_len || i < ksl_len; i++) {
        if(i < knl_len) {
            bal[2*i] = knl[i];
        } else {
            bal[2*i] = 0;
        }
        if(i < ksl_len) {
            bal[2*i+1] = ksl[i];
        } else {
            bal[2*i+1] = 0;
        }
    }
    uint64_t order = i-1;
    for(unsigned int j = 0, fact = 1; j < i; fact *= ++j) {
        bal[2*j] /= fact;
        bal[2*j+1] /= fact;
    }
    tracking_elements_allocate_element(elements, 5+(2*i));
    tracking_elements_add_datum_u64(elements, MultipoleID);
    tracking_elements_add_datum_u64(elements, order);
    tracking_elements_add_datum_f64(elements, length);
    tracking_elements_add_datum_f64(elements, hxl);
    tracking_elements_add_datum_f64(elements, hyl);
    for(unsigned int j = 0; j < 2*i; j++) {
        tracking_elements_add_datum_f64(elements, bal[j]);
    }
}

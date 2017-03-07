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


#include "track.h"

#ifndef _GPUCODE

#include <math.h>
#include <stdio.h>
#define M_PI 3.14159265358979323846

#endif

#define CLIGHT 299792458

#include "particle.h"


// Parse Element records (as described in track_element_definitions.h):

/* Do a first pass to generate the type_t enum:
 * this is done by picking out only the first id argument from each Element macro,
 * concatting it (using ##) with the token 'ID,',
 * and sticking the resulting comma-separated token inside an enum declaration.
*/
#define Element(id, stdef, func) id ## ID,

typedef enum type_t {
    IntegerID,
    DoubleID,
    #include TRACK_ELEMENT_DEFS
    BlockID,
    ElementsCount
} type_t;

#undef Element


/* Do another pass to generate the element structs.
 * This is done by picking out the first two arguments from each Element macro (id and stdef),
 * and using them to generate the struct body.
 * _(t, l) and _M(t, l) in this context do the same thing,
 * which is just concat t and l together to form a variable declaration.
 * A helper macro, UNPAREN, is used to strip the parentheses around the stdef argument so it can be properly pasted into a struct definition.
*/
#define Element(id, stdef, func) \
    typedef struct { \
        UNPAREN stdef \
    } ST_##id;
#define _(t, l) t l
#define _M(t, l) _(t, l)
#define UNPAREN(x) x

#include TRACK_ELEMENT_DEFS

#undef UNPAREN
#undef _M
#undef _
#undef Element


/* Do a final pass to generate the track functions.
 * In this context, _(t, l) expands into a variable initialization (t l = el->l),
 * and _M(t, l) expands as empty, so that the field variable can be manually initialized later as needed.
 * The func argument needs to be surrounded by parentheses so that any commas inside don't get parsed by the cpp as additional macro arguments.
*/
#define Element(id, stdef, func) \
    int id##_track(CLGLOBAL Particle *p, CLGLOBAL ST_##id *el) { \
        UNPAREN stdef \
        func; \
    }
#define UNPAREN(x) x
#define _(t, l) t l = el->l
#define _M(t, l)

#include TRACK_ELEMENT_DEFS

#undef UNPAREN
#undef _M
#undef _
#undef Element

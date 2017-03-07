#include "track.h"


/* Element dispatch parsing:
 * This generates a switch statement from the element definitions,
 * that will call the respective element type's track function
 * given an element identifier (enum type_t).
 *
 * Before including this file,
 * it is necessary to define the following macros:
 *    - TRACK_DISPATCH_ID: the element type variable that is checked
 * TRACK_DISPATCH_ELEM: a pointer variable to the element data
 * TRACK_DISPATCH_PART: a pointer variable to the particle being acted on
*/
#if defined(TRACK_DISPATCH_ID) && defined(TRACK_DISPATCH_ELEM) && defined(TRACK_DISPATCH_PART)


#define Element(id, stdef, func) \
    case id##ID: \
        id##_track(TRACK_DISPATCH_PART, (CLGLOBAL ST_##id*) TRACK_DISPATCH_ELEM); \
        break;

switch(TRACK_DISPATCH_ID) {
    #include TRACK_ELEMENT_DEFS
    default: break;
}

#undef Element
#undef TRACK_DISPATCH_ID
#undef TRACK_DISPACH_ELEM
#undef TRACK_DISPATCH_PART


#endif

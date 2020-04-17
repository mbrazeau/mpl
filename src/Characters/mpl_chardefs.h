//
//  mpl_chardegs.h
//  Morphy
//
//  Created by Martin Brazeau on 02/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_chardegs_h
#define mpl_chardegs_h

#include <limits.h>
#include <stdlib.h>

#include "mpl_defs.h"

typedef unsigned int mpl_discr;
#define MAXSTATES (CHAR_BIT * sizeof(mpl_discr) - 1U)  // -1 for the NA reserved token

#define NA       ((mpl_discr)1U) // Bitwise representation of the inapplicable symbol
#define ISAPPLIC ((mpl_discr)~NA) // Bitmask for states that are applicable
#define MISSING  ((mpl_discr)~0) // Bitmask for missing data (all state)
#define UNKNOWN  ((mpl_discr)~NA) // Bitmask for missing data that's know to be logically applicable
#define NOCHARS  0UL // Empty set
#define GAPPUSH  1 // Shift value for the position of the gap

#define VALID_NEXMAT_PUNC   "{}();"
#define VALID_XREAD_MATPUNC "[];"
#define VALID_MPL_MATPUNC   "{}[]();"
#define VALID_WILDCARD      "-?+"
#define VALID_STATESYMB     "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define VALID_WS            "\n\t "
#define VALIDSYMB   VALID_NEXMAT_PUNC VALID_XREAD_MATPUNC VALID_WILDCARD \
VALID_STATESYMB VALID_WS

static inline unsigned int mpl_discr_hibit (unsigned int n)
{
    n |= (n >>  1);
    n |= (n >>  2);
    n |= (n >>  4);
    n |= (n >>  8);
    n |= (n >> 16);
    return n - (n >> 1);
}


#endif /* mpl_chardegs_h */

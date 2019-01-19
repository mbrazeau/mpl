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

typedef enum {
    
    MPL_DISCR_T,
    MPL_CONTIN_T,
    MPL_MODEL_T
    
} mpl_data_t;

typedef enum {
    
    MPL_NOTPARSIM,
    MPL_FITCH_T,
    MPL_WAGNER_T,
    // TODO: Other parsimony types
    
} mpl_parsim_t;

typedef enum {
    
    GAP_INAPPLIC,
    GAP_MISSING,
    GAP_NEWSTATE,
    
    GAP_MAX,
    
} mpl_gap_t;

typedef unsigned long mpl_discr;
#define MAXSTATES (CHAR_BIT * sizeof(mpl_discr) - 1UL)  // -1 for the NA reserved token

#define NA       ((mpl_discr)1UL) // Bitwise representation of the inapplicable symbol
#define ISAPPLIC ((mpl_discr)~NA) // Bitmask for states that are applicable
#define MISSING  ((mpl_discr)UINT_MAX) // Bitmask for missing data (all state)
#define UNKNOWN  ((mpl_discr)~NA) // Bitmask for missing data that's know to be logically applicable
#define GAPPUSH  1 // Shift value for the position of the gap

#define DEFAULT_MISSING_SYMB '?'
#define DEFAULT_UNKNOWN_SYMB '+'
#define DEFAULT_GAP_SYMB     '-'
#define DEFAULT_DATA_T       MPL_DISCR_T
#define DEFAULT_PARSIM_T     MPL_FITCH_T
#define DEFAULT_GAP_T        GAP_INAPPLIC

#define VALID_NEXMAT_PUNC   "{}();"
#define VALID_XREAD_MATPUNC "[];"
#define VALID_MPL_MATPUNC   "{}[]();"
#define VALID_WILDCARD      "-?"
#define VALID_STATESYMB     "+0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define VALID_WS            "\n\t "
#define VALIDSYMB   VALID_NEXMAT_PUNC VALID_XREAD_MATPUNC VALID_WILDCARD \
VALID_STATESYMB VALID_WS

#endif /* mpl_chardegs_h */

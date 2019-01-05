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

typedef unsigned long   mpl_discr;

#define NA      ((mpl_discr)1UL)
#define MISSING ((mpl_discr)~0UL)
#define UNKNOWN ((mpl_discr)~NA)

#define MAXSTATES (CHAR_BIT * sizeof(mpl_discr) - 1UL)  // -1 for the NA reserved token

#define DEFAULT_MISSING '?'
#define DEFAULT_UNKNOWN '+'
#define DEFAULT_GAP     '-'

#define VALID_NEXMAT_PUNC   "{}();"
#define VALID_XREAD_MATPUNC "[];"
#define VALID_WILDCAR       "-?"
#define VALID_STATESYMB   "+0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
#define VALID_WS            "\n\t "
#define VALIDSYMB   VALID_NEXMAT_PUNC VALID_XREAD_MATPUNC VALID_WILDCAR \
VALID_STATESYMB VALID_WS

#endif /* mpl_chardegs_h */

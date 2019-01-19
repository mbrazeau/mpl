//
//  mpl_defs.h
//  Morphy
//
//  Created by Martin Brazeau on 05/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_defs_h
#define mpl_defs_h

//#include "Taxa/mpl_taxblock.h"
//#include "Search/mpl_search.h"
//#include "Results/mpl_results.h"

#if defined(__GNUC__)
#define MPL_POPCOUNTLL(c, v) (c = __builtin_popcountl(v))
#else
#define MPL_POPCOUNTLL(c, v)    \
v = v - ((v >> 1) & (unsigned long)~(unsigned long)0/3);\
v = (v & (unsigned long)~(unsigned long)0/15*3) \
    + ((v >> 2) & (unsigned long)~(unsigned long)0/15*3);\
v = (v + (v >> 4)) & (unsigned long)~(unsigned long)0/255*15;\
c = (unsigned long)(v * ((unsigned long)~(unsigned long)0/255)) \
    >> (sizeof(unsigned long) - 1) * CHAR_BIT;
#endif

typedef enum {
    
    MPL_SUCCESS         =  0,
    MPL_ERR             = -1,
    MPL_NOMEM           = -2,
    MPL_OUTOFBOUNDS     = -3,
    MPL_UNEXPNULLPTR    = -4,
    MPL_NOTIMPLEMENTED  = -5,
    MPL_NODIMENSIONS    = -6,  // Input matrix has no known dimensions
    MPL_ILLEGOVERWRITE  = -7,  // Attempt to inexplicitly overwrite data
    MPL_BADPARAM        = -8
    
} MPL_RETURN;

typedef struct _taxablock   mpl_taxablock;
typedef struct _search      mpl_search;
typedef struct _results     mpl_results;
typedef struct _matrix      mpl_matrix;

typedef struct _handle {
    
    mpl_taxablock*  taxablock;
    mpl_search*     search;
    mpl_results*    results;
    mpl_matrix*     matrix;
    
} mpl_handle;


#endif /* mpl_defs_h */

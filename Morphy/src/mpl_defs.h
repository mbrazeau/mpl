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

#define MPL_RAND_MAX        2147483646U
#define MPL_DEFAULT_RSEED   1U
#define MPL_EPSILON         (1E-15) // Could be as low as 30, but this is safe for now
#define MPL_MAX_DEPENDECIES 2 // Maximum number of root characters

typedef enum {
    
    MPL_SUCCESS         =  0,
    MPL_ERR             = -1,
    MPL_NOMEM           = -2,
    MPL_OUTOFBOUNDS     = -3,
    MPL_UNEXPNULLPTR    = -4,
    MPL_NOTIMPLEMENTED  = -5,
    MPL_NODIMENSIONS    = -6,  // Input matrix has no known dimensions
    MPL_ILLEGOVERWRITE  = -7,  // Attempt to inexplicitly overwrite data
    MPL_BADPARAM        = -8,
    MPL_NOSEMICOLON     = -9,
    MPL_PARENTHMSM      = -10,
    MPL_BADDIMENSIONS   = -11,
    MPL_BADSYMBOL       = -12
    
} MPL_RETURN;

typedef enum {
    
    MPL_DISCR_T     = 0,
    MPL_CONTIN_T    = 1,
    MPL_MODEL_T     = 2,
    
    MPL_DATA_T_MAX
    
} mpl_data_t;

typedef enum {
    
    MPL_FITCH_T     = 0,
    MPL_WAGNER_T    = 1,
    // TODO: Other parsimony types
    
    MPL_PARSIM_T_MAX
    
} mpl_parsim_t;

typedef enum {
    
    OPTIM_PARSIM        = 0,
    OPTIM_LIKELIHOOD    = 1, // Ambitious... I know... - MDB
    
    OPTIM_MAX
    
} mpl_optim_t;

typedef enum {
    
    GAP_INAPPLIC    = 0,
    GAP_MISSING     = 1,
    GAP_NEWSTATE    = 2,
    
    GAP_MAX,
    
} mpl_gap_t;

typedef enum
{
    MPL_AST_SIMPLE,
    MPL_AST_RANDOM,
    MPL_AST_ASIS,
    MPL_AST_CLOSEST,
    
    MPL_AST_MAX
    
} mpl_stepw_t;


#endif /* mpl_defs_h */

//
//  mpl_defs.h
//  Morphy
//
//  Created by Martin Brazeau on 05/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_defs_h
#define mpl_defs_h

#include <float.h>
//#include "Taxa/mpl_taxblock.h"
//#include "Search/mpl_search.h"
//#include "Results/mpl_results.h"
#ifdef _WIN32
#define  restrict __restrict
#else
#define  restrict
#endif

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

#define RET_IF_NULL(p) if (p == NULL) {return MPL_UNEXPNULLPTR;}


#define MPL_RAND_MAX        2147483646U
#define MPL_EPSILON         (1E-15) // Could be as low as 30, but this is safe for now
#define MPL_MAXSCORE        DBL_MAX
#define MPL_MAX_DEPENDECIES 2 // Maximum number of root characters

// Define all defaults:
#define MPL_DEFAULT_RSEED    1U
#define MPL_DEFAULT_MAXTREES 5000
#define MPL_DEFAULT_AUTOINCR 100
#define DEFAULT_NREPS        10
#define DEFAULT_RATCHITER    50
#define DEFAULT_MISSING_SYMB '?'
#define DEFAULT_UNKNOWN_SYMB '+'
#define DEFAULT_GAP_SYMB     '-'
#define DEFAULT_DATA_T       MPL_DISCR_T
#define DEFAULT_PARSIM_T     MPL_FITCH_T
#define DEFAULT_GAP_T        GAP_INAPPLIC
#define DEFAULT_WEIGHT       1.0000000000
#define DEFAULT_PREWEIGHT    0.2000000000
#define DEFAULT_BBREAK       MPL_TBR_T
#define DEFAULT_ASTYPE       MPL_AST_ASIS
#define DEFAULT_HOLD         ((int)1)


typedef enum {
    
    MPL_SUCCESS         =  0,  // No error
    MPL_ERR             = -1,  // General error
    MPL_NOMEM           = -2,  // Insufficient memory
    MPL_OUTOFBOUNDS     = -3,  // Subscript or index is out of buffer's bounds
    MPL_UNEXPNULLPTR    = -4,  // Function expects non-NULL pointer
    MPL_NOTIMPLEMENTED  = -5,  // The requested routine is not implemented
    MPL_NODIMENSIONS    = -6,  // Input matrix has no known dimensions
    MPL_ILLEGOVERWRITE  = -7,  // Attempt to inexplicitly overwrite data
    MPL_BADPARAM        = -8,  // Some inappropriate parameter passed to the function
    MPL_NOSEMICOLON     = -9,  // Expected terminal semicolon in input
    MPL_PARENTHMSM      = -10, // Mismatched parentheses
    MPL_BADDIMENSIONS   = -11, // Input matrix has dimensions error
    MPL_BADSYMBOL       = -12, // Input uses unrecognised symbol
    MPL_NOTREES         = -13, // There are no trees in memory
    MPL_NOTREADY        = -14  // Dataset not ready for attempted operation
    
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
    
    GAP_MAX
    
} mpl_gap_t;

typedef enum {
    
    MPL_AST_RANDOM,
    MPL_AST_ASIS,
    MPL_AST_INMEM,
    
    MPL_AST_MAX
    
} mpl_addseq_t;

typedef enum {
    
    MPL_EXHAUSTIVE_T,
    MPL_BANDB_T,
    MPL_HEURISTIC_T,
    
    MPL_SEARCH_MAX
    
} mpl_search_t;

typedef enum {
    
    MPL_NNI_T,
    MPL_SPR_T,
    MPL_TBR_T,
    
    MPL_BBREAK_MAX
    
} mpl_bbreak_t;

typedef enum {
    MPL_STRICT_T,
    MPL_MAJRULE_T,
    
    MPL_CONTREE_MAX
} mpl_contree_t;

#endif /* mpl_defs_h */

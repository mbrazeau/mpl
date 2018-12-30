//
//  mpl_search.h
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef mpl_search_h
#define mpl_search_h

#include "../Results/mpl_results.h"

typedef enum {
    MPL_EXHAUSTIVE_T,
    MPL_BANDB_T,
    MPL_HEURISTIC_T
} mpl_search_t;

typedef enum {
    MPL_NNI_T,
    MPL_SPR_T,
    MPL_TBR_T
} mpl_bbreak_t;

typedef struct _search {
    
} mpl_search;

void            mpl_do_stepwise(void);
void            mpl_do_bbreak(void);
void            mpl_do_heuristic(void);
mpl_results*    mpl_do_search(void);

#endif /* mpl_search_h */

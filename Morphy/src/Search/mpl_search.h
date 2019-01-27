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
#include "../Trees/mpl_node.h"
#include "../Trees/mpl_tree.h"
#include "../Trees/mpl_treelist.h"

#define DEFAUL_BBREAK MPL_TBR_T

typedef enum {
    MPL_INPUTORDER_T,
    MPL_RANDOM_T,
    MPL_DISTANCE_T
} mpl_stepwise_t;

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
    
    mpl_search_t    search_type;
    mpl_bbreak_t    bbreak_type;
    long            hold;
    long            num_reps;
    mpl_treelist*   treelist;
    
} mpl_search;

//void            mpl_do_stepwise(void);
//void            mpl_do_bbreak(void);
//void            mpl_do_heuristic(void);
mpl_results*    mpl_do_search(void);

#endif /* mpl_search_h */

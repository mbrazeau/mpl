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

typedef struct _search mpl_search;
typedef void (*mpl_searchfxn)(mpl_search* s);

typedef struct _search {
    
    mpl_search_t    search_type;
    mpl_bbreak_t    bbreak_type;
    mpl_stepwise_t  stepw_type;
    long            hold;
    long            num_reps;
    mpl_treelist*   treelist;
    mpl_searchfxn   searchfxn;
    mpl_tree*       constraint;
    
} mpl_search;

int             mpl_search_set_search_t(const mpl_search_t stype, mpl_search* s);
int             mpl_search_set_bbreak_t(const mpl_bbreak_t bbkt, mpl_search* s);
int             mpl_search_set_stepw_t(const mpl_stepwise_t swtype, mpl_search* s);
int             mpl_search_set_hold(const int hold, mpl_search* s);
int             mpl_search_add_treelist(mpl_treelist* tl, mpl_search* s);
mpl_results*    mpl_do_search(mpl_search* s);

#endif /* mpl_search_h */

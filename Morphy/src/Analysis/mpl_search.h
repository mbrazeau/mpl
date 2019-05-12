//
//  mpl_search.h
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef mpl_search_h
#define mpl_search_h

#include "../mpl_defs.h"
#include "../Results/mpl_results.h"
#include "../Trees/mpl_node.h"
#include "../Trees/mpl_tree.h"
#include "../Trees/mpl_treelist.h"
#include "../Analysis/mpl_scoretree.h"


typedef struct _search mpl_search;
typedef struct _bbreak mpl_bbreak;

typedef int (*mpl_searchfxn)(mpl_search* s);

typedef struct _search {
    
    long            num_taxa;
    mpl_search_t    search_type;
    mpl_bbreak_t    bbreak_type;
    mpl_addseq_t    stepw_type;
    int             hold;
    long            keep;
    long            num_reps;
    int             ratch_iter;
    bool            use_ratchet;
    mpl_treelist*   treelist;
    mpl_bbreak*     bbreak;
    mpl_searchfxn   searchfxn;
    mpl_tree*       tree;
    mpl_tree*       constraint;
    
} mpl_search;

mpl_search*     mpl_search_new(const long ntax, mpl_treelist* tl);
void            mpl_search_delete(mpl_search** s);

int             mpl_search_set_search_t(const mpl_search_t stype, mpl_search* s);
int             mpl_search_set_bbreak_t(const mpl_bbreak_t bbkt, mpl_search* s);
int             mpl_search_set_stepw_t(const mpl_addseq_t swtype, mpl_search* s);
int             mpl_search_set_hold(const int hold, mpl_search* s);
int             mpl_search_add_treelist(mpl_treelist* tl, mpl_search* s);
int             mpl_search_set_numreps(const long nreps, mpl_search* s);
bool            mpl_search_is_reptype(mpl_search* s);
int             mpl_search_use_ratchet(const bool useratchet, mpl_search* s);
MPL_RETURN      mpl_search_execute(mpl_search* s);

#endif /* mpl_search_h */

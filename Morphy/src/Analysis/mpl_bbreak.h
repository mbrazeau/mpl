//
//  mpl_bbreak.h
//  Morphy
//
//  Created by Martin Brazeau on 07/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_bbreak_h
#define mpl_bbreak_h

#include <stdbool.h>

#include "../mpl_defs.h"
#include "mpl_search.h"
#include "../mpl_handle.h"
#include "../Trees/mpl_node.h"
#include "../Trees/mpl_tree.h"
//#include "../Trees/mpl_topol.h"
#include "../Trees/mpl_treelist.h"
#include "mpl_stepwise.h"

typedef struct _bbreak {
    
    long            numtaxa;
    long            numreps;
    bool            hitisland;
    mpl_bbreak_t    bbktype;
    long            num_nodes;
    mpl_node**      clips;
    mpl_node**      srcs;
    long            nlongtgts;
    mpl_node**      tgtslong;
    long            nshorttgts;
    mpl_node**      tgtsshort;
    unsigned long   num_rearrangs;
    mpl_treelist*   treelist;
    mpl_stepwise    stepwise;
    
    double          keepscore; // Allowance for longer trees
    double          limit;  // Overall limit
    double          shortest; // Shortest overall
    double          bestinrep; // Shortest in a given replicate

} mpl_bbreak;

mpl_bbreak* mpl_bbreak_new(long num_nodes);
void        mpl_bbreak_delete(mpl_bbreak** bbk);
int         mpl_bbreak_init(mpl_search* s, mpl_bbreak* bbk);
void        mpl_bbreak_reset(mpl_bbreak* bbk);
void        mpl_branch_swap(mpl_tree* t, mpl_bbreak* bbk);
void        mpl_do_bbreak(mpl_bbreak* bbk);

#endif /* mpl_bbreak_h */

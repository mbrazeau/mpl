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

#include "mpl_defs.h"
#include "mpl_search.h"
#include "mpl_handle.h"
#include "mpl_node.h"
#include "mpl_tree.h"
//#include "../Trees/mpl_topol.h"
#include "mpl_treelist.h"
#include "mpl_stepwise.h"

typedef struct _bbreak {
    
    long            numtaxa;
    long            numreps;
    long            nratchets; // Number of ratchet iterations
    long            savelim;
    long            savecount;
    int             nsubs;
    int             maxsubs;
    int             maxhits;
    int             nhits;
    bool            hitisland;
    bool            doislandcheck;
    mpl_bbreak_t    bbktype;
    long            num_nodes;
    mpl_node**      clips;
    mpl_node**      srcs;
    long            nlongtgts;
    mpl_node**      tgtslong;
    long            nshorttgts;
    mpl_node**      tgtsshort;
    unsigned long   num_rearrangs; // TODO: Need to support 32-bit int range because this will easily overflow on 32-bit machine
    mpl_treelist*   treelist;
    mpl_topol*      head;
    mpl_stepwise    stepwise;
    
    double          keepscore; // Allowance for longer trees
    double          limit;  // Overall limit
    double          shortest; // Shortest overall
    double          bestinrep; // Shortest in a given replicate

} mpl_bbreak;

mpl_bbreak* mpl_bbreak_new(long num_nodes);
void        mpl_bbreak_delete(mpl_bbreak** bbk);
int         mpl_bbreak_init(mpl_search* s, mpl_bbreak* bbk);
int         mpl_bbreak_search(mpl_search* s);
void        mpl_bbreak_cleanup(mpl_bbreak* bbk);
void        mpl_branch_swap(mpl_tree* t, mpl_bbreak* bbk);
void        mpl_bbreak_set_type(const mpl_bbreak_t bbkt, mpl_bbreak* bbk);
void        mpl_do_bbreak(mpl_bbreak* bbk);

#endif /* mpl_bbreak_h */

//
//  mpl_stepwise.h
//  Morphy
//
//  Created by Martin Brazeau on 25/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_stepwise_h
#define mpl_stepwise_h

#include "../mpl_defs.h"
#include "../Trees/mpl_tree.h"
#include "../Trees/mpl_topol.h"
#include "../Trees/mpl_treelist.h"

typedef struct mpl_stepwise {
    
    mpl_addseq_t    astype;
    long            num_tips;
    long            tips_added;
    long*           addseq;
    int             hold;
    mpl_node*       next;
    long            nsites;
    mpl_node**      sites;
    mpl_tree*       tree;
    double          shortest;
    double          longest;
    mpl_treelist*   held;
    mpl_treelist*   queued;
    
} mpl_stepwise;


void mpl_stepwise_init
(const mpl_addseq_t astype, const long num_taxa, const int hold, mpl_stepwise* sw);
void mpl_stepwise_reset(mpl_stepwise* sw);
void mpl_stepwise_set_type(const mpl_addseq_t astype, mpl_stepwise* sw);
mpl_addseq_t mpl_stepwise_get_type(mpl_stepwise* sw);
void mpl_stepwise_do_search(mpl_stepwise* sw);

#endif /* mpl_stepwise_h */

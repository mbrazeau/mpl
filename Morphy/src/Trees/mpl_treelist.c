//
//  mpl_treelist.c
//  Morphy
//
//  Created by Martin Brazeau on 09/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <assert.h>

#include "../mpl_utils.h"
#include "mpl_treelist.h"

static void mpl_treelist_resize(long num_taxa, long extension, mpl_treelist* tl);

mpl_treelist* mpl_treelist_new(long num_taxa, long max_trees, long increase_rate)
{
    mpl_treelist* tl = NULL;
    
    if ((tl = (mpl_treelist*)safe_calloc(1, sizeof(mpl_treelist)))) {
        tl->num_trees = 0;
        tl->max_trees = max_trees;
        tl->increase_rate = increase_rate;
        
        
    }
    
    return tl;
}

void mpl_treelist_delete(mpl_treelist** tl)
{
    // TODO: Implement
}

long mpl_add_tree(mpl_tree* t, mpl_treelist* tl)
{
    
    if (!(tl->num_trees <= tl->max_trees)) {
        if (tl->increase_rate == 0) {
            return -1;
        }
        
        // TODO: Check return of this
        mpl_treelist_resize(tl->trees[0].num_taxa, tl->increase_rate, tl);
    }
    // TODO: Check bounds and decide whether to extend
    mpl_topol* top = &tl->trees[tl->num_trees];
    
    mpl_tree_record_topol(top, t);
    
    ++tl->num_trees;
    
    assert(tl->num_trees <= tl->max_trees);
    // TODO: Rebase the topology if the tree is unrooted
    // TODO: This can only be done once you decide how to record topologies
    // which may have less than the maximum number of tips...
    
    
    return 0;
}

mpl_topol* mpl_treelist_get_topol(long tnum, mpl_treelist* tl);

/*
 *  PRIVATE FUNCTION DEFINITIONS
 */

// TODO: This really needs a return to check.
static void mpl_treelist_resize(long num_taxa, long extension, mpl_treelist* tl)
{
    size_t i = 0;
    
    if (tl->trees == NULL) {
        tl->trees = (mpl_topol*)safe_calloc(tl->max_trees, sizeof(mpl_topol));
    }
    else {
        mpl_topol* extlist = NULL;
        
        extlist = (mpl_topol*)realloc(tl->trees, (tl->max_trees + extension) * sizeof(mpl_topol));
        
        i = tl->max_trees;
        tl->max_trees += extension;
    }
    
    for ( ; i < tl->max_trees; ++i) {
        // TODO: This needs an opposite destructor function for if the process
        // fails. Also check returns from topol_init.
        mpl_topol_init(num_taxa, &tl->trees[i]);
    }
}

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
void mpl_treelist_extend(const long nelems, mpl_treelist* tl);

mpl_treelist*
mpl_treelist_new
(const long num_taxa, const long max_trees, const long increase_rate)
{
    mpl_treelist* tl = NULL;
    
    if ((tl = (mpl_treelist*)safe_calloc(1, sizeof(mpl_treelist)))) {
        
        tl->num_taxa        = num_taxa;
        tl->num_trees       = 0;
        tl->max_trees       = max_trees;
        tl->increase_rate   = increase_rate;
        
        // TODO: Check return from this:
        mpl_treelist_resize(num_taxa, max_trees, tl);
    }
    
    return tl;
}

void mpl_treelist_delete(mpl_treelist** tl)
{
    // TODO: Implement
}

long mpl_treelist_add_tree(const bool checkdupes, mpl_tree* t, mpl_treelist* tl)
{
    long i = 0;
    
    if (!(tl->num_trees < tl->max_trees)) {
        if (tl->increase_rate == 0) {
            return -1;
        }
        
        // TODO: Check return of this
        mpl_treelist_resize(tl->trees[0].num_taxa, tl->increase_rate, tl);
        
//        mpl_treelist_extend(tl->increase_rate, tl);
    }
    // TODO: Check bounds and decide whether to extend
    mpl_topol* top = &tl->trees[tl->num_trees];
    
    mpl_tree_record_topol(top, t);
    
    if (checkdupes == true) {
        for (i = 0; i < tl->num_trees; ++i) {
            if (!mpl_topol_compare(top, &tl->trees[i])) {
                return 1;
            }
        }
    }
    
    tl->back = top;
    ++tl->num_trees;
    if (tl->num_trees == 1) {
        tl->head = top;
    }
    
    assert(tl->num_trees <= tl->max_trees);
    // TODO: Rebase the topology if the tree is unrooted
    // TODO: This can only be done once you decide how to record topologies
    // which may have less than the maximum number of tips...
    
    
    return 0;
}

long mpl_treelist_get_numtrees(const mpl_treelist* tl)
{
    return tl->num_trees;
}

void mpl_treelist_overwrite_longest(mpl_tree* t, mpl_treelist* tl)
{
    long i = 0;
    long lcount = 0;
    double biglen = 0.0;
    double shortlen = 0.0;
    long longest = 0;
    
    if (tl->num_trees == tl->max_trees) {
        shortlen = tl->trees[0].score;
        for (i = 0; i < tl->num_trees; ++i) {
            if (tl->trees[i].score > biglen) {
                biglen = tl->trees[i].score;
                longest = i;
                lcount = 1;
            }
            else if (tl->trees[i].score == biglen) {
                ++lcount;
            }
            else if (tl->trees[i].score < shortlen) {
                shortlen = tl->trees[i].score;
            }
        }
    }
    
    if (t->score > biglen && biglen != 0.0 && tl->num_trees >= tl->max_trees) {
        return;
    }
    
//    if (tl->num_trees == tl->max_trees) {
//        if (biglen == shortlen) {
//            // Randomly break ties.
//            assert(lcount > -1);
//            unsigned choice = mpl_rng_between(0, (unsigned)lcount+1);
//            if (choice == lcount) {
//                // The choice is the new tree, so just exit
//                return;
//            }
//            long j = 0;
//            for (i = 0; i < tl->num_trees; ++i) {
//                if (tl->trees[i].score == biglen) {
//                    if (j == choice) {
//                        longest = choice;
//                        break;
//                    }
//                    ++j;
//                }
//            }
//        }
//    }
    
    mpl_topol* top = &tl->trees[longest];
    mpl_tree_record_topol(top, t);
    
    if (tl->num_trees == 0) {
        tl->back = &tl->trees[0];
        tl->head = tl->back;
        ++tl->num_trees;
    }
}

mpl_topol* mpl_treelist_get_topol(long tnum, mpl_treelist* tl)
{
    if (tnum < tl->num_trees) {
        return &tl->trees[tnum];
    }
    
    return NULL;
}

mpl_topol* mpl_treelist_get_next(mpl_treelist* tl)
{
    mpl_topol* ret = NULL;
    ret = tl->head;
    if (tl->head > tl->back) {
        tl->head = NULL;
        ret = NULL;
    }
    ++tl->head;
    
    return ret;
}

mpl_topol* mpl_treelist_get_shortest(mpl_treelist* tl)
{
    long i = 0;
    double shortest;
    
    mpl_topol* ret = 0;
    ret = &tl->trees[0];
    shortest = ret->score;
    
    for (i = 1; i < tl->num_trees; ++i) {
        if (tl->trees[i].score < shortest) {
            ret = &tl->trees[i];
            shortest = ret->score;
        }
    }
    
    return ret;
}

void mpl_treelist_clear_all(mpl_treelist* tl)
{
    tl->num_trees = 0;
    tl->shortest = 0.0;
    tl->longest = 0.0;
    tl->head = NULL;
    tl->back = NULL;
}

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
        
        if (extlist != NULL) {
            tl->trees = extlist;
        }
        
        i = tl->max_trees;
        tl->max_trees += extension;
    }
    
    for ( ; i < tl->max_trees; ++i) {
        // TODO: This needs an opposite destructor function for if the process
        // fails. Also check returns from topol_init.
        // TODO: Make sure this isn't leaking memory by cutting off last topology
        // in the list
        tl->trees[i].edges = NULL;
        mpl_topol_init(num_taxa, &tl->trees[i]);
    }
}

void mpl_treelist_push_back(mpl_topol* top, mpl_treelist* tl)
{
    if (tl->back != NULL) {
        mpl_topol_link(tl->back, top);
        
    }
    else {
        tl->front = top;
        top->back = NULL;
    }
    
    tl->back = top;
}

void mpl_treelist_extend(const long nelems, mpl_treelist* tl)
{
    
    long i = 0;
    long limit = 0;
    mpl_topol* nt = NULL;
    mpl_topol* en = NULL;
    
    // Check if the requested number of elements will exceed maxtrees
    // and if that maximum is allowed to be extended.
    if ((tl->num_trees + nelems) > tl->max_trees) {
        if (tl->increase_rate == 0) {
            limit = tl->max_trees - tl->num_trees;
        }
    }
    else {
        limit = nelems;
        tl->max_trees += nelems;
    }
    
    // If the request amounts to zero, then do nothing.
    if (!limit) {
        return;
    }
    
    // Set up the start of the linked list
    nt = mpl_topol_new(tl->num_taxa);
    
    // Check the list isn't empty
    if (tl->back != NULL) {
        mpl_topol_link(tl->back, nt);
        tl->back = nt;
    }
    else {
        assert(tl->front == NULL);
        tl->back = tl->front = nt;
    }
    
    do {
        en = NULL;
        en = mpl_topol_new(tl->num_taxa);
        mpl_topol_init(tl->num_taxa, en);
        mpl_topol_link(nt, en);
        nt = en;
        en = en->next;
        tl->back = nt;
        ++i;
    } while (i < nelems);
    
    assert(en == NULL);

}

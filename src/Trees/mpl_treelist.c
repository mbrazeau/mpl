//
//  mpl_treelist.c
//  Morphy
//
//  Created by Martin Brazeau on 09/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <stdio.h>
#include <assert.h>

#include "mpl_utils.h"
#include "mpl_treelist.h"

static int mpl_treelist_resize(long num_taxa, long extension, mpl_treelist* tl);
void mpl_treelist_extend(const long nelems, mpl_treelist* tl);

mpl_treelist* mpl_treelist_new(const long num_taxa, const long max_trees, const long increase_rate)
{
    mpl_treelist* tl = NULL;
    
    if ((tl = (mpl_treelist*)safe_calloc(1, sizeof(mpl_treelist)))) {
        tl->num_taxa = num_taxa;
        tl->num_trees = 0;
        tl->max_trees = max_trees;
        tl->increase_rate = increase_rate;
        tl->repstart = NULL;
        tl->rep_num_trees = 0;
        
        // TODO: Check return from this:
        mpl_treelist_resize(num_taxa, max_trees, tl);
    }
    
    return tl;
}

void mpl_treelist_delete(mpl_treelist** tl)
{
    mpl_treelist* tli = NULL;
    tli = *tl;
    if (tli != NULL) {
        
        long i = 0;
        
        for (i = 0; i < (*tl)->max_trees; ++i) {
            mpl_topol_cleanup(&(*tl)->trees[i]);
        }
        
        safe_free((*tl)->trees);
    }
}

void mpl_treelist_set_maxtrees(const long maxtrees, mpl_treelist* tl)
{
    if (maxtrees < tl->num_trees) {
        tl->num_trees = tl->max_trees;
        tl->back = &tl->trees[tl->num_trees-1];
        
        if (tl->head > tl->back) {
            tl->head = NULL;
        }
    }
    
    if (maxtrees > tl->max_trees) {
        mpl_treelist_resize(tl->num_taxa, maxtrees - tl->max_trees, tl);
    }
    
    printf("Resize tree list to %lu trees for a total of: ", tl->max_trees);
    size_t tlbytes = 0;
    tlbytes += sizeof(mpl_topol);
    tlbytes += tl->trees[0].num_nodes * sizeof(long);
    tlbytes *= tl->max_trees;
    printf("%lu bytypes\n", tlbytes);
    
}

mpl_topol* mpl_treelist_add_tree(const bool checkdupes, mpl_tree* t, mpl_treelist* tl)
{
    long i = 0;
    
    if (tl->num_trees >= tl->max_trees) {
        
        if (tl->increase_rate == 0) {
            return NULL;
        }
        
        // TODO: Check return of this
        mpl_treelist_resize(tl->trees[0].num_taxa, tl->increase_rate, tl);
    }

    mpl_topol* top = &tl->trees[tl->num_trees];
    
    mpl_tree_record_topol(top, t);
    
    if (checkdupes == true) {
        for (i = 0; i < tl->num_trees; ++i) {
            if (!mpl_topol_compare(top, &tl->trees[i])) {
                return &tl->trees[i];
            }
        }
    }
    
    tl->back = top;
    ++tl->num_trees;
    ++tl->rep_num_trees;
    
    if (tl->num_trees == 1 || tl->head == NULL) {
        tl->head    = top;
        tl->front   = &tl->trees[0];
    }
    
    assert(tl->num_trees <= tl->max_trees);
    // TODO: Rebase the topology if the tree is unrooted
    // TODO: This can only be done once you decide how to record topologies
    // which may have less than the maximum number of tips...
    
    
    return NULL;
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


void mpl_treelist_reset_head(mpl_treelist* tl)
{
    tl->head = tl->front;
}

void mpl_treelist_reverse_head(mpl_treelist* tl)
{
    if (tl->head != NULL) {
        if (tl->head == tl->front) {
            tl->head = NULL;
        }
        else {
            --tl->head;
        }
    }
    else {
        tl->head = tl->back;
    }
}

mpl_topol* mpl_treelist_get_next(mpl_treelist* tl)
{
    mpl_topol* ret = NULL;
    
    if (tl->head != NULL) {
        
        ret = tl->head;
        
        if (tl->head == tl->back) {
            tl->head = NULL;
        }
        else {
            ++tl->head;
        }
    }
    
    return ret;
}

mpl_topol* mpl_treelist_get_shortest(mpl_treelist* tl)
{
    long i = 0;
    double shortest;
    
    mpl_topol* ret = NULL;
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
    tl->num_trees       = 0;
    tl->rep_num_trees   = 0;
    tl->shortest        = 0.0;
    tl->longest         = 0.0;
    tl->head            = NULL;
    tl->back            = NULL;
    tl->repstart        = NULL;
    
}

void mpl_treelist_restart_rep(mpl_treelist* tl)
{
    tl->head = &tl->trees[tl->num_trees-1];
}

void mpl_treelist_clear_rep(mpl_treelist* tl)
{
    tl->num_trees = tl->num_trees - tl->rep_num_trees;
    tl->rep_num_trees = 0;
    assert(tl->num_trees >= 0);
    if (tl->num_trees > 0) {
        tl->back = &tl->trees[tl->num_trees-1];
    }
    else {
        tl->back = NULL;
    }
    tl->head = NULL;
//    tl->back = tl->head;
}

mpl_topol* mpl_treelist_new_subrep(mpl_treelist* tl)
{
    tl->head = tl->repstart;
    tl->rep_num_trees = 0;
    
    return tl->head;
}

mpl_topol* mpl_treelist_newrep(bool checknew, mpl_tree* t, mpl_treelist* tl)
{
    mpl_topol* ret = NULL;
    
    tl->rep_num_trees   = 0;
    
    ret = mpl_treelist_add_tree(checknew, t, tl);
    
    if (ret == NULL || checknew == false) {
        tl->repstart = tl->head;
        tl->head = tl->back;
        tl->rep_index = tl->back->index;
        ret = tl->head;
    }
    else {
        ret = NULL;
    }
    
    return ret;
}



/*
 *  PRIVATE FUNCTION DEFINITIONS
 */

// TODO: This really needs a return to check.
static int mpl_treelist_resize(long num_taxa, long extension, mpl_treelist* tl)
{
    size_t i = 0;
    size_t cleanupstart = 0;
    
    if (tl->trees == NULL) {
        tl->trees = (mpl_topol*)safe_calloc(tl->max_trees, sizeof(mpl_topol));
    }
    else {
        
        long ihdead = 0;
        long iback = 0;
        if (tl->head != NULL) {
            ihdead = tl->head->index;
        }
        if (tl->back != NULL) {
            iback = tl->back->index;
        }
        mpl_topol* extlist = NULL;
        
        extlist = (mpl_topol*)realloc(tl->trees, (tl->max_trees + extension) * sizeof(mpl_topol));
        
        if (extlist != NULL) {
            tl->trees = extlist;
            if (tl->front != NULL) {
                tl->front = &tl->trees[0];
            }
            if (tl->head != NULL) {
                tl->head = &tl->trees[ihdead];
            }
            if (tl->back != NULL) {
                tl->back = &tl->trees[iback];
            }
        }
        
        else {
            return -1;
        }
        
        i = tl->max_trees;
        tl->max_trees += extension;
    }
    
    cleanupstart = i;
    
    for ( ; i < tl->max_trees; ++i) {
        // TODO: This needs an opposite destructor function for if the process
        // fails. Also check returns from topol_init.
        // TODO: Make sure this isn't leaking memory by cutting off last topology
        // in the list
        tl->trees[i].index = i;
        tl->trees[i].edges = NULL;
        
        if (mpl_topol_init(num_taxa, &tl->trees[i])) {
            // Cleanup;
            mpl_topol* topptr = NULL;
            for (i = cleanupstart; i < tl->max_trees; ++i) {
                topptr = &tl->trees[i];
                mpl_topol_delete(&topptr);
            }
            tl->max_trees = cleanupstart;
            // Exit
            return -1;
        }
    }
    
    return 0;
}

//void mpl_treelist_push_back(mpl_topol* top, mpl_treelist* tl)
//{
//    if (tl->back != NULL) {
//        mpl_topol_link(tl->back, top);
//
//    }
//    else {
//        tl->front = top;
//        top->back = NULL;
//    }
//
//    tl->back = top;
//}

//void mpl_treelist_extend(const long nelems, mpl_treelist* tl)
//{
//    
//    long i = 0;
//    long limit = 0;
//    mpl_topol* nt = NULL;
//    mpl_topol* en = NULL;
//    
//    // Check if the requested number of elements will exceed maxtrees
//    // and if that maximum is allowed to be extended.
//    if ((tl->num_trees + nelems) > tl->max_trees) {
//        if (tl->increase_rate == 0) {
//            limit = tl->max_trees - tl->num_trees;
//        }
//    }
//    else {
//        limit = nelems;
//        tl->max_trees += nelems;
//    }
//    
//    // If the request amounts to zero, then do nothing.
//    if (!limit) {
//        return;
//    }
//    
//    // Set up the start of the linked list
//    nt = mpl_topol_new(tl->num_taxa);
//    
//    // Check the list isn't empty
//    if (tl->back != NULL) {
//        mpl_topol_link(tl->back, nt);
//    }
//    else {
//        assert(tl->front == NULL);
//        tl->back = tl->front = nt;
//    }
//    
//    do {
//        en = mpl_topol_new(tl->num_taxa);
//        mpl_topol_init(tl->num_taxa, en);
//        mpl_topol_link(nt, en);
//        nt = en;
//        en = en->next;
//        tl->back = nt;
//        ++i;
//    } while (i < nelems);
//    
//    assert(en == NULL);
//
//}

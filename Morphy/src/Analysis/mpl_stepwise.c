//
//  mpl_stepwise.c
//  Morphy
//
//  Created by Martin Brazeau on 25/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <string.h>
#include <stdio.h>

#include "../mpl_utils.h"
#include "mpl_stepwise.h"
#include "mpl_scoretree.h"

static void mpl_shuffle_addseq(mpl_stepwise* sw);
static void mpl_set_addition_sequence(mpl_stepwise* sw);
static void mpl_setup_first_fork(mpl_stepwise* sw);
static void mpl_switch_tree_buffers(mpl_stepwise* sw);
static void mpl_store_starttree_nodes(mpl_tree* t, mpl_stepwise *sw);
static void mpl_try_all_sites(mpl_node* n, mpl_tree* t, mpl_stepwise *sw);

/*******************************************************************************
 *                                                                             *
 *  PRIVATE FUNCTION DEFINITIONS                                               *
 *                                                                             *
 ******************************************************************************/
void mpl_stepwise_init
(const mpl_stepw_t astype, const long num_taxa, const int hold, mpl_stepwise* sw)
{
    long i = 0;
    
    sw->astype      = astype;
    sw->num_tips    = num_taxa;
    sw->hold        = hold;
    
    sw->addseq      = (long*)safe_calloc(num_taxa, sizeof(long));
    sw->sites       = (mpl_node**)safe_calloc(2 * num_taxa, sizeof(mpl_node*));
    
    for (i = 0; i < num_taxa; ++i) {
        sw->addseq[i] = i;
    }
    
    // TODO: Check these returns:
    sw->tree    = mpl_new_tree(num_taxa);
    sw->held    = mpl_treelist_new(num_taxa, hold, 0);
    sw->queued  = mpl_treelist_new(num_taxa, hold, 0);
}


void mpl_stepwise_reset(mpl_stepwise* sw)
{
    sw->astype      = DEFAULT_ASTYPE;
    sw->num_tips    = 0;
    sw->hold        = DEFAULT_HOLD;
    safe_free(sw->addseq);
    mpl_delete_tree(&sw->tree);
}


void mpl_stepwise_set_type(const mpl_stepw_t astype, mpl_stepwise* sw)
{
    sw->astype = astype;
}


mpl_stepw_t mpl_stepwise_get_type(mpl_stepwise* sw)
{
    return sw->astype;
}


void mpl_stepwise_set_hold(const int hold, mpl_stepwise* sw)
{
    sw->hold = hold;
}


int mpl_stepwise_get_hold(mpl_stepwise *sw)
{
    return sw->hold;
}


void mpl_stepwise_set_num_tips(const long numtips, mpl_stepwise* sw)
{
    sw->num_tips = numtips;
}


long mpl_stepwise_get_num_tips(mpl_stepwise* sw)
{
    return sw->num_tips;
}


void mpl_stepwise_do_search(mpl_stepwise* sw)
{
    long i = 0;
    long j = 0;
    long nsaved = 0;
    long nheld;
    mpl_node* nexttip = NULL;
    
    sw->longest = 0.0;
    sw->shortest = 0.0;
    
    mpl_set_addition_sequence(sw);
    mpl_setup_first_fork(sw);
    mpl_treelist_add_tree(false, sw->tree, sw->queued);
    
    // TODO: If there's only three taxa, you can skip most of this stuff
    // and just return the arbitrary 3-taxon tree.
    
    // Copy the first fork into the saved trees
    mpl_topol* nexttopol = NULL;
//    mpl_tree_record_topol(nexttopol, sw->tree);
//    nsaved = 1;
    
    for (i = sw->tips_added; i < sw->num_tips; ++i) {
        
        mpl_treelist_clear_all(sw->held);
        // Push the tip onto a new base, so that it can be joined to the
        // tree
        nexttip = &sw->tree->nodes[sw->addseq[i]];

        nsaved = sw->queued->num_trees;
        
        // For all the trees in the queue
        for (j = 0; j < nsaved; ++j) {
            
            nexttopol = mpl_treelist_get_topol(0, sw->queued);
            
            mpl_tree_read_topol(sw->tree, nexttopol);
            
            mpl_node_push_desc(sw->next, nexttip);
            // try nexttip in all the places in the tree
            mpl_try_all_sites(nexttip, sw->tree, sw);
        }
        
        ++sw->next;
        // Swap results and queue bufers
        mpl_switch_tree_buffers(sw);
        
    }

}



/*******************************************************************************
 *                                                                             *
 *  PRIVATE FUNCTION DEFINITIONS                                               *
 *                                                                             *
 ******************************************************************************/
static void mpl_shuffle_addseq(mpl_stepwise* sw)
{
    long i;
    long j;
    long t = 0;
    
    for (i = 0; i < sw->num_tips; ++i) {
        j = i + mpl_rng() / (MPL_RAND_MAX / (sw->num_tips) + 1);
        t = sw->addseq[j];
        sw->addseq[j] = sw->addseq[i];
        sw->addseq[i] = t;
    }
}

static void mpl_set_addition_sequence(mpl_stepwise* sw)
{
    switch (sw->astype) {
        case MPL_AST_RANDOM:
            mpl_shuffle_addseq(sw);
            break;
        default:
            break;
    }
}

static void mpl_setup_first_fork(mpl_stepwise* sw)
{
    // Set up the 'next' pointer to the first available internal node
    sw->next = &sw->tree->nodes[sw->tree->num_taxa];
    
    // For an unrooted starting tree, build on the base
    mpl_node_push_desc(sw->tree->dummy, sw->next);
    sw->tree->base = sw->next;
    ++sw->next;
    
    // Push the first branch onto the base
    mpl_node_push_desc(sw->tree->base, &sw->tree->nodes[sw->addseq[0]]);
    // Push the first node onto the base
    mpl_node_push_desc(sw->tree->base, sw->next);
    // Push the next two branches onto the node
    mpl_node_push_desc(sw->next, &sw->tree->nodes[sw->addseq[1]]);
    mpl_node_push_desc(sw->next, &sw->tree->nodes[sw->addseq[2]]);
    ++sw->next;
    sw->tips_added = 3;
    
    // Traverse the tree and setup the first postorder list
    mpl_tree_traverse(sw->tree);
}

static void mpl_switch_tree_buffers(mpl_stepwise* sw)
{
    mpl_treelist* t;
    t = sw->held;
    sw->held = sw->queued;
    sw->queued = t;
}

static void mpl_store_starttree_nodes(mpl_tree* t, mpl_stepwise *sw)
{
    long i;;
    long j;
    mpl_tree_traverse(t);
    sw->nsites = t->size-2;
    for (i = 0, j = 1; i < sw->nsites; ++i, ++j) {
        sw->sites[i] = t->postord_all[j];
    }
}

static void mpl_try_all_sites
(mpl_node* n, mpl_tree* t, mpl_stepwise *sw)
{
    long i = 0;
    long nsites = 0;
    
    double bestlength = 0.0;
    double longest = t->score;
    
    // Store all the sites locally, as traversals on the tree during
    // checking may modify the list.
    mpl_store_starttree_nodes(t, sw);
    nsites = sw->nsites;
    
    // First do the first site and set the best length
    mpl_node_bin_connect(sw->sites[0], NULL, n);
    longest = bestlength = t->score = mpl_fullpass_parsimony(t);
    if (longest > sw->longest) {
        sw->longest = longest;
    }
//    printf("Try length: %f\n", t->score);
    mpl_treelist_add_tree(false, t, sw->held);
    mpl_node_bin_clip(n);
    
    for (i = 1; i < nsites; ++i) {
        
        // Test insertion at
        mpl_node_bin_connect(sw->sites[i], NULL, n);
        
        t->score = mpl_fullpass_parsimony(t);
        
        if (t->score < bestlength) {
            bestlength = t->score;
            mpl_treelist_overwrite_longest(t, sw->held);

            if (bestlength < sw->shortest) {
                sw->shortest = bestlength;
            }
        }
        else if (t->score == bestlength) {
            mpl_treelist_overwrite_longest(t, sw->held);
        }
        else if (t->score <= sw->longest) {

            if (sw->held->num_trees < sw->held->max_trees) {
                 mpl_treelist_add_tree(false, t, sw->held);
            }
            else {
                mpl_treelist_overwrite_longest(t, sw->held);
            }
        }
        else {
            longest = t->score;
            if (longest > sw->longest) {
                sw->longest = longest;
            }
        }

        mpl_node_bin_clip(n);
    }
}

/*
 *  TEST INTERFACE FUNCTION DEFINITIONS
 */

#ifdef DEBUG
void mpl_test_setup_first_fork(mpl_stepwise* sw)
{
    mpl_setup_first_fork(sw);
}
#endif

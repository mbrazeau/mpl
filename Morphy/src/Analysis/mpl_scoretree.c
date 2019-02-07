//
//  mpl_scoretree.c
//  Morphy
//
//  Created by Martin Brazeau on 25/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <assert.h>

#include "../Characters/mpl_matrix.h"
#include "../Trees/mpl_tree.h"
#include "../Characters/mpl_parsim.h"
#include "mpl_scoretree.h"

mpl_matrix* glmatrix = NULL;

void mpl_init_parsimony(mpl_matrix* m)
{
    glmatrix = m;
}

double mpl_fullpass_parsimony(mpl_tree* t)
{
    double len = 0.0;
    long i = 0;
    mpl_node* n;
    
    mpl_tree_traverse(t);
    
    mpl_parsim_reset_scores(glmatrix);
    
    // Downpass
    for (i = 0; i < t->nintern; ++i) {
        
        n = t->postord_intern[i];
        
        len += mpl_parsim_first_downpass(n->left->mem_index,
                                         n->right->mem_index,
                                         n->mem_index, glmatrix);
    }
    
    n = t->postord_intern[i-1];
    mpl_parsim_do_root(n->mem_index, n->anc->mem_index, glmatrix);
    
    // Uppass
    for (i = t->size; i--; ) {
        
        n = t->postord_all[i];
        
        if (n->tip == 0) {
            mpl_parsim_first_uppass(n->left->mem_index, n->right->mem_index,
                                    n->mem_index, n->anc->mem_index, glmatrix);
        }
        else {
            mpl_parsim_tip_update(n->mem_index, n->anc->mem_index, glmatrix);
        }
    }
    
    
    if (glmatrix->gaphandl == GAP_INAPPLIC) {
        
        // Downpass for inapplicables
        for (i = 0; i < t->nintern; ++i) {
            n = t->postord_intern[i];
            
            len += mpl_parsim_second_downpass (n->left->mem_index,
                                               n->right->mem_index,
                                               n->mem_index, glmatrix);
        }
        
        // Do the root
        n = t->postord_intern[i-1];
        mpl_parsim_do_root(n->mem_index, n->anc->mem_index, glmatrix);
        
        // Uppass
        for (i = t->size; i--; ) {
            n = t->postord_all[i];
            if (n->tip == 0) {
                mpl_parsim_second_uppass
                (n->left->mem_index, n->right->mem_index,
                 n->mem_index, n->anc->mem_index, glmatrix);
            }
            else {
                mpl_parsim_tip_finalize(n->mem_index, n->anc->mem_index, glmatrix);
            }
        }
    }
    
//    mpl_charbuf_assert_temps_equal_bufs(&glmatrix->cbufs[MPL_DISCR_T]);
    // Buffer the original state sets
//    mpl_scoretree_copy_original_characters();
    
    return len;
}


/**
 This function  does a partial uppass on the tree and should be called first
 on the deepest node in the tree to be affected by a branch insertion. The
 result of this function fould be a full set of nodes requiring a second
 uppass update should this be necessary when working with inapplicable
 characters.
 
 @param n Pointer to the node being visited on this call.
 @param ostart Constant pointer to the original starting position
 @param i A pointer to the index of the current number of nodes stored.
 @param t A pointer to the tree being worked on.
 */
void mpl_part_parsim_uppass
(mpl_node* n, const mpl_node* ostart, long* i, mpl_tree* t)
{
    if (n->tip > 0) {
        mpl_na_only_parsim_tip_update(n->mem_index, n->anc->mem_index, glmatrix);
        return;
    }
    
    // Partial up function:
    // mpl_parsim_update_first_uppass(n->left->mem_index, n->right->mem_index,
    //                                n->mem_index, n->anc->mem_index, glmatrix)
    // if (!mpl_check_updated(n->mem_index, glmatrix) && n != ostart) {
    //      return;
    // }
//    if (!mpl_na_only_parsim_first_uppass(n->left->mem_index, n->right->mem_index, n->mem_index, n->anc->mem_index, glmatrix))
//    {
//        return;
//    }

    mpl_na_only_parsim_first_uppass(n->left->mem_index, n->right->mem_index, n->mem_index, n->anc->mem_index, glmatrix);
    
//    t->partial_pass[*i] = n;
//    ++(*i);
    
//    if (n->anc == ostart) {
//        return;
//    }
    
    mpl_part_parsim_uppass(n->left, ostart, i, t);
    mpl_part_parsim_uppass(n->right, ostart, i, t);
}

double mpl_fullpass_parsimony_na_only(mpl_node* start, mpl_tree* t)
{
    double len = 0.0;
    long i = 0;
    mpl_node* n;
    
    mpl_tree_traverse(t);
    
//     Downpass
//    for (i = 0; i < t->nintern; ++i) {
//
//        n = t->postord_intern[i];
//
//        mpl_na_only_parsim_first_downpass(n->left->mem_index,
//                                         n->right->mem_index,
//                                         n->mem_index, glmatrix);
//    }

    n = start;

    while (n->anc != NULL) {
        double chgs = 0.0;
        chgs = mpl_na_only_parsim_first_downpass(n->left->mem_index,
                                          n->right->mem_index,
                                          n->mem_index, glmatrix);
        if (chgs == 0.0) {
            break;
        }
        n = n->anc;
    };

    
    if (n == t->base->anc) {
        n = t->base;//t->postord_intern[i-1];
        mpl_na_only_parsim_do_root(n->mem_index, n->anc->mem_index, glmatrix);
    }
//
    mpl_part_parsim_uppass(n, NULL, NULL, t);
//    // Uppass
//    for (i = t->size; i--; ) {
//
//        n = t->postord_all[i];
//
//        if (n->tip == 0) {
//            mpl_na_only_parsim_first_uppass(n->left->mem_index, n->right->mem_index,
//                                    n->mem_index, n->anc->mem_index, glmatrix);
//        }
//        else { //if (n != start ){
//            mpl_na_only_parsim_tip_update(n->mem_index, n->anc->mem_index, glmatrix);
//        }
//    }
    
    // Downpass for inapplicables
    for (i = 0; i < t->nintern; ++i) {
        n = t->postord_intern[i];
        
        len += mpl_na_only_parsim_second_downpass(n->left->mem_index,
                                                  n->right->mem_index,
                                                  n->mem_index, glmatrix);
    }
    
    mpl_parsim_reset_root_state_buffers(t->base->mem_index, t->base->anc->mem_index, glmatrix);
//    mpl_parsim_reset_state_buffers(glmatrix);
    
    return len;
}

double mpl_partpass_parsimony(mpl_node* start, mpl_tree* t)
{
    double len = 0.0;
    long i = 0;
    mpl_node* n;

    // Copy the current state sets into the temp buffers
    // ...
    
    // Do a downpath from the clipsite
    n = start;
    do {
        // Perform partial downpass calculations and check for updates
        // If the length already exceeds the limit, you can exit here
        n = n->anc;
    } while (n->tip != -1);
    
    // If n->tip == -1, then you hit the bottom. Do the root

    // Perform a recursive uppass on this
    // void mpl_part_parsim_uppass(mpl_node* n, const mpl_node* ostart, long* i, mpl_tree* t)
    // i = 0;
    // mpl_part_parsim_uppass(n, start, &i, t);
    
    
    // Downpass for inapplicables
    // For now, this should be a full downpass, working only on those inapplic
    // characters that cannot be resolved at a local insertion

    // Restore to the original state sets.
    
    return len;
}


/**
 Performs a fast reoptimisation of a clipped tree, updating state sets that
 have been affected by clipping

 @param src Pointer to the base node of the clipped subtree.
 @param csite Pointer to the sister node of the clipped subtree.
 @param t Pointer to the tree struct itself.
 */
double mpl_clipped_tree_parsimony(mpl_node* src, mpl_node* csite, mpl_tree* t)
{
    double score = 0.0; // Sum of scores of clipped subtrees
    
    // FOR NOW: Do full-pass on both subtrees:
    // This requires a modification of how downpass sets are collected
    return score;
}


/**
 Calculates the length of a proposed reinsertion. This function first tests
 standard parsimony characters without completing the reinsertion. When
 non-standard (i.e. inapplicable) characters need to be checked, the branch
 will be reinserted and a partial pass will be made on the tree, updating any
 characters as necessary. If no limit is set, then then function will perform
 calculations for all character sets. If a limit is indicated, then the function
 will return as soon as the limit is hit.

 @param lim A cutoff value (limit) that determines when to stop checking
 character sets. Set to a negative value if no limit is to be used.
 @param src Pointer to the base node of the clipped subtree.
 @param tgt Pointer to the target site on the tree.
 @param t Pointer to the tree struct itself.
 @return The calculated length added to the tree by proposed move.
 */
double mpl_score_try_parsimony
(const double lim, mpl_node* src, mpl_node* tgt, mpl_tree* t)
{
    double score = 0.0;
//    double oldnascore = 0.0;
    double scorerecall = 0.0;
    
    // Do the fast check on any characters that can be compared quickly at
    // this junction.
    // If the lim is set and the score exceeds the limit already, return score.
    score       = mpl_parsim_local_check
                 (lim, src->mem_index, tgt->mem_index, tgt->anc->anc->mem_index, glmatrix);
    scorerecall = mpl_parsim_get_score_recall(glmatrix);

    
    if (glmatrix->gaphandl == GAP_INAPPLIC) {
//        mpl_scoretree_restore_original_characters();
//        oldnascore = mpl_parsim_get_na_scores(glmatrix);
//        assert(scorerecall == oldnascore);
//        mpl_scoretree_restore_original_characters();
        score -= scorerecall;
        score += mpl_fullpass_parsimony_na_only(src->anc, t);
//        mpl_scoretree_restore_original_characters();
//        mpl_parsim_reset_state_buffers(glmatrix);
    }
    
    // Otherwise, check if the sum of score of the standard characters and the
    // minimum possible score of the inapplicable characters exceeds the limit.
    // If so, abort try and move on.
//    double stdtryscore = 0.0;
//    stdtryscore = mpl_parsim_get_standard_tryscore(glmatrix);
    
    
    // Otherwise, temporarily insert the branch at tgt.
    
    // Create a simple downpass set of nodes on the path down from the insertion
    // site to the root and perform a downpass operation on these nodes,
    // updating only those characters affected by clipping.
    
    // Using the resultant set of nodes, begin an uppass, but add to the list
    // any nodes affected by uppass reconstructions.
    
    // Perform a downpass from these nodes, adding to the length any states
    // needing update.
    
    // Remove src from the tgt site.
    
    // Restore the state sets from the temp buffers
//    mpl_scoretree_restore_original_characters();
    
    return score;
}

void mpl_scoretree_copy_original_characters(void)
{
    mpl_charbuf_store_discr_states(&glmatrix->cbufs[MPL_DISCR_T]);
}


void mpl_scoretree_restore_original_characters(void)
{
    mpl_parsim_swap_stateset_ptrs(&glmatrix->cbufs[MPL_DISCR_T]);
//    mpl_charbuf_restore_discr_states(&glmatrix->cbufs[MPL_DISCR_T]);
//    mpl_parsim_reset_state_buffers(glmatrix);
//    if (glmatrix->nparsets > 1) {
//        mpl_charbuf_fast_restore_discr_states
//        (glmatrix->parsets[1].end - glmatrix->parsets[1].start, glmatrix->parsets[1].indexbuf, &glmatrix->cbufs[MPL_DISCR_T]);
//    }
}

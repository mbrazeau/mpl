//
//  mpl_scoretree.c
//  Morphy
//
//  Created by Martin Brazeau on 25/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#include <float.h>

#include "mpl_matrix.h"
#include "mpl_tree.h"
#include "mpl_parsim.h"
#include "mpl_scoretree.h"

static void mpl_part_parsim_uppass(mpl_node* n, const mpl_node* ostart, long* i, mpl_tree* t);

mpl_matrix* glmatrix = NULL;

MPL_RETURN mpl_init_parsimony(mpl_matrix* m)
{
    RET_IF_NULL(m);
    
    glmatrix = m;
    
    return MPL_SUCCESS;
}

long mpl_fullpass_parsimony(mpl_tree* t)
{
    long len = 0;
    long i = 0;
    mpl_node* n;

    mpl_tree_traverse(t);
    
    mpl_parsim_reset_scores(glmatrix);
//    mpl_parsim_zero_na_nodal_changes(start->anc->mem_index, glmatrix);
    
    // Downpass
    for (i = 0; i < t->nintern; ++i) {
        
        n = t->postord_intern[i];
        
        n->score = mpl_parsim_first_downpass(n->left->mem_index,
                                         n->right->mem_index,
                                         n->mem_index, glmatrix);
        len += n->score;
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
        mpl_parsim_finalize_root(n->mem_index, n->anc->mem_index, glmatrix);
        
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
    
    return len;
}

long mpl_length_only_parsimony(const long lim, mpl_tree* t)
{
    long len = 0;
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
            if (len > lim) {
                return len;
            }
        }
    }
    
    return len;
}

void mpl_traverse_subtree(mpl_node* n, mpl_node** internals, mpl_node** allnodes, long* i, long *j)
{
    if (n->tip > 0) {
        allnodes[*i] = n;
        (*i)++;
        return;
    }
    
    mpl_traverse_subtree(n->left, internals, allnodes, i, j);
    mpl_traverse_subtree(n->right, internals, allnodes, i, j);
    
    allnodes[*i] = n;
    internals[*j] = n;
    (*i)++;
    (*j)++;
}


long mpl_fullpass_subtree(mpl_node* subtr, mpl_tree* t)
{
    long len = 0;
    long i = 0;
    mpl_node* n = NULL;
    
    long internnds = t->nintern; // TODO: This is unsafe. Needs to know prior call to full traversal
    long alln = t->size;

    mpl_traverse_subtree
    (subtr, t->postord_intern, t->postord_all, &alln, &internnds);
    
    // Downpass
    for (i = t->nintern; i < internnds; ++i) {
        
        n = t->postord_intern[i];
        
        len += mpl_parsim_first_downpass(n->left->mem_index,
                                         n->right->mem_index,
                                         n->mem_index, glmatrix);
    }
    
//    n = t->postord_intern[internnds-1];//subtr;//internalnodes[i-1];
    mpl_parsim_do_root(n->mem_index, n->anc->mem_index, glmatrix);
    
    // Uppass
    for (i = alln; i > t->size-1; i--) {
        
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
        for (i = t->nintern; i < internnds; ++i) {
            
            n = t->postord_intern[i];//t->postord_intern[i];
            
            len += mpl_parsim_second_downpass (n->left->mem_index,
                                               n->right->mem_index,
                                               n->mem_index, glmatrix);
        }
        
        // Do the root
        n = t->postord_intern[i-1];//t->postord_intern[i-1];
        mpl_parsim_finalize_root(n->mem_index, n->anc->mem_index, glmatrix);
        
        // Uppass
        for (i = alln; i > t->size-1; i--) {
            
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
 This function does a partial uppass on the tree and should be called first
 on the deepest node in the tree to be affected by a branch insertion. The
 result of this function fould be a full set of nodes requiring a second
 uppass update should this be necessary when working with inapplicable
 characters.
 
 @param n Pointer to the node being visited on this call.
 @param ostart Constant pointer to the original starting position
 @param i A pointer to the index of the current number of nodes stored.
 @param t A pointer to the tree being worked on.
 */
static void mpl_part_parsim_uppass
(mpl_node* n, const mpl_node* ostart, long* i, mpl_tree* t)
{
    if (n->tip > 0) {
        mpl_na_only_parsim_tip_update(n->mem_index, n->anc->mem_index, glmatrix);
        return;
    }

    if (!mpl_na_only_parsim_first_uppass(n->left->mem_index,
        n->right->mem_index, n->mem_index, n->anc->mem_index, glmatrix)) {

        if ((!n->marked) && (n != ostart)) {
            return;
        }
        
    }
    
    mpl_part_parsim_uppass(n->left, ostart, i, t);
    mpl_part_parsim_uppass(n->right, ostart, i, t);
    
    t->partial_pass[*i] = n;
    ++(*i);
}


void mpl_partial_update_uppass(mpl_node* n)
{
    if (n->tip > 0) {

        // TODO: Only need to call the inapplicable function
        mpl_parsim_tip_update(n->mem_index, n->anc->mem_index, glmatrix);
        return;
    }
    
    // TODO: Only need to call the inapplicable function
    mpl_parsim_first_uppass(n->left->mem_index, n->right->mem_index, n->mem_index, n->anc->mem_index, glmatrix);
    
    mpl_partial_update_uppass(n->left);
    mpl_partial_update_uppass(n->right);
}


void mpl_update_src_actives(mpl_node* start, const mpl_node* end)
{
    mpl_node* n = start;

    // Update the root:
    // TODO: Write a function to update the prupset of the new root
    mpl_parsim_update_root_prupset(end->left->mem_index, end->right->mem_index, end->mem_index, glmatrix);
    // Then do this crap:
    do {
        mpl_parsim_update_active_sets(n->left->mem_index, n->right->mem_index, n->mem_index, glmatrix);
        n = n->anc;
    } while (n != end->anc);

}


void mpl_src_redo_downpass(mpl_node* n)
{
    if (n->tip > 0) {
        return;
    }
    
    mpl_src_redo_downpass(n->left);
    mpl_src_redo_downpass(n->right);
    
    mpl_na_only_parsim_first_downpass(n->left->mem_index,
                                      n->right->mem_index,
                                      n->mem_index, glmatrix);
}


void mpl_src_root_parsimony(mpl_node* src)
{
    if (src->tip > 0) {
        mpl_parsim_do_src_tip(src->mem_index, glmatrix);
    }
    else {
        mpl_parsim_do_src_root(src->left->mem_index, src->right->mem_index, src->mem_index, glmatrix);
    }
}

void mpl_use_ndidx(const bool useflag)
{
    mpl_parsim_use_nodeidx(useflag, glmatrix);
}

long mpl_fullpass_parsimony_na_only(const long lim, mpl_node* start, mpl_tree* t)
{
    long len = 0;
    long i = 0;
    mpl_node* n;
    mpl_node* p;
    mpl_node* t1 = NULL;
    mpl_node* t2 = NULL;
    long chgs = 0;
    
    mpl_tree_traverse(t);
    
    mpl_parsim_zero_na_nodal_changes(start->anc->mem_index, glmatrix);
//    mpl_parsim_reset_nodal_indexbufs(glmatrix);
    
    t1 = mpl_node_get_sib(start);
    t2 = start->anc->anc;
    
    if (start->tip > 0) {
        n = start->anc;
    }
    else {
        n = start;
    }
    
    while (n->anc != NULL) {
        p = n;
        chgs = mpl_na_only_parsim_first_downpass(n->left->mem_index,
                                          n->right->mem_index,
                                          n->mem_index, glmatrix);
        n->marked = 1;
        
        if (chgs == 0 && n != start && n != start->anc) {
            break;
        }
        
        n = n->anc;
    };

    if (n == t->base->anc) {
        n = t->base;//t->postord_intern[i-1];
        mpl_na_only_parsim_do_root(n->mem_index, n->anc->mem_index, glmatrix);
    }

    t->nsubnodes = 0;
    mpl_part_parsim_uppass(n, start, &t->nsubnodes, t);
    
    len = 0;
    mpl_use_ndidx(true);
    
    for (i = 0; i < t->nsubnodes; ++i) {

        n = t->partial_pass[i];
    
        if (!n->marked) {
            mpl_use_ndidx(true);
        } else {
            mpl_use_ndidx(false);
        }
        
        len += mpl_na_only_parsim_second_downpass(n->left->mem_index,
                                                  n->right->mem_index,
                                                  n->mem_index,
                                                  glmatrix);

        if (lim > -1) {
            if (len > lim) {
                // Copy the temp buffers back in and exit the loop
                for ( ; i < t->nsubnodes; ++i) {
                    n = t->partial_pass[i];
                    mpl_parsim_reset_root_state_buffers(n->left->mem_index,
                                                        n->right->mem_index,
                                                        glmatrix);
                }
                mpl_parsim_reset_root_state_buffers(n->mem_index,
                                                    n->anc->mem_index,
                                                    glmatrix);
                break;
            }
        }
    }

    mpl_use_ndidx(false);
    n = n->anc;
    
    if (len <= lim || lim < 0) {
        while (n != t->base->anc) {
            len += mpl_na_only_parsim_second_downpass(n->left->mem_index,
                                                      n->right->mem_index,
                                                      n->mem_index,
                                                      glmatrix);
            t->partial_pass[t->nsubnodes] = n;
            ++t->nsubnodes;

            if (lim > -1) {
                if (len > lim) {
                    while (n != t->base->anc){
                        n = t->partial_pass[i];
                        mpl_parsim_reset_root_state_buffers(n->left->mem_index,
                                                            n->right->mem_index,
                                                            glmatrix);
                        n = n->anc;
                    }
                    mpl_parsim_reset_root_state_buffers(n->mem_index,
                                                        n->anc->mem_index,
                                                        glmatrix);
                    break;
                }
            }

            n = n->anc;
        };  
    }

    if (n == t->base->anc) {
        n = t->base;
        mpl_parsim_reset_root_state_buffers(n->mem_index, n->anc->mem_index, glmatrix);
    }

    // Unmark all the nodes
    for (i = 0; i < t->num_nodes; ++i) {
        t->nodes[i].marked = 0;
    }

    return len;
}

long mpl_partpass_parsimony(mpl_node* start, mpl_tree* t)
{
    long len = 0;
//    long i = 0;
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
long mpl_clipped_tree_parsimony(mpl_node* src, mpl_node* csite, mpl_tree* t)
{
    long score = 0; // Sum of scores of clipped subtrees
    
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
long mpl_score_try_parsimony
(const long sttlen, const long lim, mpl_node* src, mpl_node* tgt, mpl_tree* t)
{
    long score       = 0;
    long minscore    = 0;
    long scorerecall = 0;
    long diff        = -1;
    
    // Do the fast check on any characters that can be compared quickly at
    // this junction.
    
    // If the lim is set and the score exceeds the limit already, return score.
    if (lim > -1) {
        diff = lim-sttlen;
    }

    // This gives a minimum number of steps added with a quick check.
    score = mpl_parsim_local_check(lim, sttlen, src->mem_index,
                                   tgt->mem_index,
                                   tgt->anc->anc->mem_index,
                                   t->base->mem_index, glmatrix);

    if (glmatrix->gaphandl == GAP_INAPPLIC) {
        
        // Get the score of all characters whose lengths can't be checked during
        // a local insertion.

        scorerecall = mpl_parsim_get_score_recall(glmatrix);

        score -= scorerecall;
        minscore = mpl_parsim_get_na_remaining_minscore(glmatrix);
        
        if (lim > -1) {
            if ((score + sttlen + minscore) > lim) {
                return score + minscore;
            }
            diff = lim - (score + sttlen);
        }
        
        score += scorerecall;
        
        score += mpl_fullpass_parsimony_na_only(lim, src, t);
    }
    
    return score;
}

long mpl_scoretree_calc_abs_minscores(void)
{
    long naminscore = 0;
    naminscore = mpl_parsim_calc_abs_minscore(glmatrix);
    glmatrix->naminscore = naminscore;
    return naminscore;
}

void mpl_scoretree_copy_original_characters(void)
{
    mpl_charbuf_store_discr_states(&glmatrix->cbufs[MPL_DISCR_T]);
}


void mpl_scoretree_restore_original_characters(mpl_matrix* m, mpl_tree* t)
{
    long i = 0;
    for (i = 0; i < t->num_nodes; ++i) {
        //
    }
}

void mpl_do_ratchet_weights(void)
{
    mpl_parsim_do_ratchet_weights(2, &glmatrix->cbufs[0]);
}

void mpl_reset_std_weights(void)
{
    mpl_parsim_reset_all_weights(&glmatrix->cbufs[0]);
}

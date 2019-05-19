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

static void mpl_part_parsim_uppass(mpl_node* n, mpl_node* ostart, long* i, mpl_tree* t);

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

double mpl_length_only_parsimony(const double lim, mpl_tree* t)
{
    double len = 0.0;
    long i = 0;
    mpl_node* n;
    
    mpl_tree_traverse(t);
    
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
//            if (len > lim) {
//                return len;
//            }
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


double mpl_fullpass_subtree(mpl_node* subtr, mpl_tree* t)
{
    double len = 0.0;
    long i = 0;
    mpl_node* n;
    
    long internnds = t->nintern;
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
    
    n = t->postord_intern[i-1];//subtr;//internalnodes[i-1];
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
static void mpl_part_parsim_uppass
(mpl_node* n, mpl_node* ostart, long* i, mpl_tree* t)
{
    if (n->tip > 0) {
        mpl_na_only_parsim_tip_update(n->mem_index, n->anc->mem_index, glmatrix);
        return;
    }

    if (!mpl_na_only_parsim_first_uppass (n->left->mem_index,
        n->right->mem_index, n->mem_index, n->anc->mem_index, glmatrix)) {

        if (!n->marked && n->anc != ostart && n != ostart && n->anc != ostart->anc) {
            return;
        }
    }
    
    n->marked = 0;
    n->anc->marked = 0;
    
    mpl_part_parsim_uppass(n->left, ostart, i, t);
    mpl_part_parsim_uppass(n->right, ostart, i, t);
    
    t->partial_pass[*i] = n;
    ++(*i);
}


void mpl_update_src_actives(mpl_node* start, const mpl_node* end)
{
    mpl_node* n = start;
    while (n != end) {
        mpl_parsim_update_active_sets(n->left->mem_index, n->right->mem_index, n->mem_index, glmatrix);
        n = n->anc;
    }
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


double mpl_fullpass_parsimony_na_only(const double lim, mpl_node* start, mpl_tree* t)
{
    double len = 0.0;
    long i = 0;
    mpl_node* n;
    mpl_node* t1 = NULL;
    mpl_node* t2 = NULL;
    double chgs = 0.0;
    
    mpl_tree_traverse(t);
    
    mpl_parsim_zero_na_nodal_changes(start->anc->mem_index, glmatrix);
    
    t1 = mpl_node_get_sib(start);
    t2 = start->anc->anc;
    
    if (start->tip > 0) {
        n = start->anc;
    }
    else {
//        mpl_src_redo_downpass(start);
        n = start;
    }

    while (n->anc != NULL) {
        chgs = mpl_na_only_parsim_first_downpass(n->left->mem_index,
                                          n->right->mem_index,
                                          n->mem_index, glmatrix);
        n->marked = 1;
        if (chgs == 0.0 && n != start && n != start->anc) {
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
    start->marked = 0;
   
    len = mpl_parsim_local_recheck(-1.0, -1.0,
                                   start->mem_index,
                                   t1->mem_index,
                                   t2->mem_index,
                                   t->base->mem_index,
                                   glmatrix);
//
    if (lim > -1.0) {
        if (len > lim) {
            // TODO: This needs a wrapper function if it's going to be more permanent
            long end = 0;
            if (glmatrix->parsets[1].nchars > 0) {
                end = glmatrix->parsets[1].indexbuf[glmatrix->parsets[1].nchars-1] + 1;
                assert(end > glmatrix->parsets[1].start);
            }
            mpl_charbuf_restore_discr_states(glmatrix->parsets[1].start, end, &glmatrix->cbufs[MPL_DISCR_T]);
            return len;
//            for (i = 0; i < t->nintern; ++i) {
//                n = t->postord_intern[i];
//                mpl_parsim_reset_root_state_buffers(n->left->mem_index, n->right->mem_index, glmatrix);
//            }
//            mpl_parsim_reset_root_state_buffers(n->mem_index, n->anc->mem_index, glmatrix);
//            return len;
        }
    }


    len = 0;
//    Downpass for inapplicables
    for (i = 0; i < t->nintern; ++i) {

        n = t->postord_intern[i];

        len += mpl_na_only_parsim_second_downpass(n->left->mem_index,
                                                  n->right->mem_index,
                                                  n->mem_index, glmatrix);
        if (lim > -1.0) {
            if (len > lim) {
//                for ( ; i < t->nintern; ++i) {
//                    n = t->postord_intern[i];
//                    mpl_parsim_reset_root_state_buffers(n->left->mem_index, n->right->mem_index, glmatrix);
//                }
//                mpl_parsim_reset_root_state_buffers(n->mem_index, n->anc->mem_index, glmatrix);
                break;
            }
        }
    }

//    if (i == t->nintern) {
//        mpl_parsim_reset_root_state_buffers(t->base->mem_index, t->base->anc->mem_index, glmatrix);
//    }
    // TODO: This needs a wrapper function if it's going to be more permanent
    long end = 0;
    if (glmatrix->parsets[1].nchars > 0) {
        end = glmatrix->parsets[1].indexbuf[glmatrix->parsets[1].nchars-1] + 1;
        assert(end > glmatrix->parsets[1].start);
    }
    mpl_charbuf_restore_discr_states(glmatrix->parsets[1].start, end, &glmatrix->cbufs[MPL_DISCR_T]);

    return len;
}

double mpl_partpass_parsimony(mpl_node* start, mpl_tree* t)
{
    double len = 0.0;
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
(const double sttlen, const double lim, mpl_node* src, mpl_node* tgt, mpl_tree* t)
{
    double score = 0.0;
    double minscore = 0.0;
    double scorerecall = 0.0;
    double diff = -1.0;
    
//    if (lim > -1.0) {
//        printf("lim: %.0f\n", lim);
//    }
    
    // Do the fast check on any characters that can be compared quickly at
    // this junction.
    // If the lim is set and the score exceeds the limit already, return score.
    if (lim > 0) {
        diff = lim-sttlen;
    }
    
//    if (src->tip == 0 && tgt->tip == 0) {
//        score = mpl_parsim_local_check_fork2fork
//        (lim, src->mem_index, src->left->mem_index, src->right->mem_index, tgt->left->mem_index, tgt->right->mem_index, tgt->mem_index, tgt->anc->anc->mem_index, tgt->anc->anc->anc->mem_index, 0, t->base->mem_index, glmatrix);
//    } else {
        score = mpl_parsim_local_check(lim, sttlen, src->mem_index,
                                        tgt->mem_index,
                                        tgt->anc->anc->mem_index,
                                        t->base->mem_index, glmatrix);
//    }


    if (glmatrix->gaphandl == GAP_INAPPLIC) {
        
        scorerecall = mpl_parsim_get_score_recall(glmatrix);

        score -= scorerecall;
        minscore = mpl_parsim_get_na_remaining_minscore(glmatrix);
        
        if (lim > -1.0) {
            
            if ((score + sttlen + minscore) > lim) {
                return score + sttlen + minscore;
            }
            
            diff = lim - (score + sttlen);
        }
        
//        score += scorerecall;
        score += mpl_fullpass_parsimony_na_only(diff, src, t);
        
    }
    
    
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

void mpl_do_ratchet_weights(void)
{
    mpl_parsim_do_ratchet_weights(&glmatrix->cbufs[0]);
}

void mpl_reset_std_weights(void)
{
    mpl_parsim_reset_all_weights(&glmatrix->cbufs[0]);
}

//
//  mpl_scoretree.c
//  Morphy
//
//  Created by Martin Brazeau on 25/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

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
        //    for (i = t->size; i--; ) {
        //        if (n->tip == 0) {
        //            mpl_parsim_second_uppass
        //            (n->left->mem_index, n->right->mem_index,
        //             n->mem_index, n->anc->mem_index, glblm);
        //        }
        //        else {
        //            // TODO: Finalize tips
        //        }
        //    }
        
        // Finalise the tips
    }
    
    return len;
}

double mpl_partpass_parsimony(mpl_node* start, mpl_tree* t)
{
    double len = 0.0;
//    long i = 0;
//    mpl_node* n;
//
//    // Do a downpath from the clipsite
//
//    // Downpass
//    for (i = 0; i < t->size; ++i) {
//        // Perform this on a partial path to the root, not the full downpass set
//    }
//
//    // Do the root
//
//    // Uppass
//    for (i = t->size; i--; ) {
//        // If internal do interal
//        // Else do tip
//    }
//
//    // Downpass for inapplicables
//    for (i = 0; i < t->size; ++i) {
//        //
//    }
//
//    // Do the root
//
//    // Uppass
//    for (i = t->size; i--; ) {
//        // If internal do interal
//        // Else do tip
//    }
    
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
    
    // Do the fast check on any characters that can be compared quickly at
    // this junction.
    // If the lim is set and the score exceeds the limit already, return score.
    
    // Otherwise, check if the sum of score of the standard characters and the
    // minimum possible score of the inapplicable characters exceeds the limit.
    // If so, abort try and move on.
    double stdtryscore = 0.0;
    stdtryscore = mpl_parsim_get_standard_tryscore(glmatrix);
    
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
    
    return score;
}

//
//  mpl_scoretree.c
//  Morphy
//
//  Created by Martin Brazeau on 25/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include "../Trees/mpl_tree.h"
#include "../Characters/mpl_parsim.h"
#include "mpl_scoretree.h"

mpl_matrix* glblm = NULL;

void mpl_init_parsimony(mpl_matrix* m)
{
    glblm = m;
}


double mpl_fullpass_parsimony(mpl_tree* t)
{
    double len = 0.0;
    long i = 0;
    mpl_node* n;
    
    mpl_tree_traverse(t);
    
    // Downpass
    for (i = 0; i < t->nintern; ++i) {
        
        n = t->postord_intern[i];
        
        len +=
        mpl_parsim_first_downpass
        (n->left->mem_index, n->right->mem_index, n->mem_index, glblm);
        
    }
    
    n = t->postord_intern[i-1];
    mpl_parsim_do_root(n->mem_index, n->anc->mem_index, glblm);
    
    // Uppass
    for (i = t->size; i--; ) {
        
        n = t->postord_all[i];
        
        if (n->tip == 0) {
            mpl_parsim_first_uppass
            (n->left->mem_index, n->right->mem_index,
             n->mem_index, n->anc->mem_index, glblm);
        }
        else {
            mpl_parsim_tip_update(n->mem_index, n->anc->mem_index, glblm);
        }
    }
    
    

    // Downpass for inapplicables
    for (i = 0; i < t->nintern; ++i) {
        n = t->postord_intern[i];

        len +=
        mpl_parsim_second_downpass
        (n->left->mem_index, n->right->mem_index, n->mem_index, glblm);
    }

    // Do the root

    // Uppass
//    for (i = t->size; i--; ) {
//
//    }

    // Finalise the tips
    
    return len;
}

double mpl_partpass_parsimony(mpl_node* start, mpl_tree* t)
{
    double len = 0.0;
    long i = 0;
    mpl_node* n;
    
    // Do a downpath from the clipsite
    
    // Downpass
    for (i = 0; i < t->size; ++i) {
        // Perform this on a partial path to the root, not the full downpass set
    }
    
    // Do the root
    
    // Uppass
    for (i = t->size; i--; ) {
        // If internal do interal
        // Else do tip
    }
    
    // Downpass for inapplicables
    for (i = 0; i < t->size; ++i) {
        //
    }
    
    // Do the root
    
    // Uppass
    for (i = t->size; i--; ) {
        // If internal do interal
        // Else do tip
    }
    
    return len;
}

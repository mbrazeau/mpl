//
//  mpl_search.c
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//
#include <assert.h>

#include "mpl_search.h"
#include "mpl_bbreak.h"
#include "mpl_stepwise.h"



mpl_results* mpl_do_search(mpl_search* s)
{
    
    s->searchfxn(s);
    
    // TODO: For now:
    return NULL;
}

MPL_RETURN mpl_search_heuristic(mpl_treelist* tl, mpl_bbreak* bbk, mpl_search* s)
{
    long reps = 0;
    
    mpl_tree* t = NULL;
    
    t = mpl_new_tree(s->num_taxa);
    
    for (reps = 0; reps < s->num_reps; ++reps) {
        
        // Get a starting tree (stepwise addition or from the existing buffer)
        
        // If doing a ratchet search: perturb the weights
        //  Calculate the length of the tree with the ratchet weights
        //  Set this to best in rep length
        //  Do branch-breaking search
        //  Reset the weights to normal
        
        // Do branch-breaking
    }
    
    return MPL_SUCCESS;
}

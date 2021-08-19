//
//  mpl_bipartlist.c
//  Morphy
//
//  Created by Martin Brazeau on 11/05/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <stdlib.h>

#include "mpl_utils.h"
#include "mpl_bipartlist.h"

static void mpl_bipartlist_cleanup(mpl_bipartlist *bpts);

mpl_bipartlist* mpl_bipartlist_new (int ntax)
{
    int i = 0;
    
    mpl_bipartlist *newbprts = safe_calloc(1, sizeof(mpl_bipartlist));
    
    if (newbprts != NULL) {
        newbprts->max_splits = 2 * ntax - 1;
        newbprts->num_splits = 0;
        
        newbprts->biparts = safe_calloc(newbprts->max_splits, sizeof(mpl_bitset*));
        if (newbprts->biparts == NULL) {
            // Cleanup & return
            mpl_bipartlist_cleanup(newbprts);
            safe_free(newbprts);
            return NULL;
        }
        
        for (i = 0; i < newbprts->max_splits; ++i) {
            newbprts->biparts[i] = mpl_bitset_new(ntax);
        }
        
        newbprts->counts = safe_calloc(newbprts->max_splits, sizeof(int));
        if (newbprts->counts == NULL) {
            // Cleanup & return
            mpl_bipartlist_cleanup(newbprts);
            safe_free(newbprts);
            return NULL;
        }
        
        newbprts->indexes = safe_calloc(newbprts->max_splits, sizeof(int));
        if (newbprts->indexes == NULL) {
            // Cleanup & return
            mpl_bipartlist_cleanup(newbprts);
            safe_free(newbprts);
            return NULL;
        }
        
    }
    
    return newbprts;
}

static void mpl_bipartlist_cleanup(mpl_bipartlist *bpts)
{
    int i = 0;
    
    for (i = 0; i < bpts->max_splits; ++i) {
        safe_free(bpts->biparts[i]);
    }
    safe_free(bpts->biparts);
    safe_free(bpts->counts);
    safe_free(bpts->indexes);
}

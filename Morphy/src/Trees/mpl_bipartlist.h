//
//  mpl_bipartlist.h
//  Morphy
//
//  Created by Martin Brazeau on 11/05/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_bipartlist_h
#define mpl_bipartlist_h

#include "../mpl_bitset.h"

typedef struct mpl_bipartlist {
    
    long        max_splits; // Max num biartitions that can be stored
    long        num_splits; // Number of biparts actually stored
    mpl_bitset* biparts;    // Buffer of bipartitions
    long*       counts;     // Counts for each bipartition for consensus trees
    
} mpl_bipartlist;

mpl_bipartlist* mpl_bipartlist_new(void);

#endif /* mpl_bipartlist_h */

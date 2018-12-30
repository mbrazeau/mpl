//
//  mpl_results.h
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef mpl_results_h
#define mpl_results_h

typedef struct _results {
    
    unsigned long   num_trees;
    unsigned long   num_islands;
    long double     num_rearrangements;
    double          besttree;
    char**          newick_trees;
    
} mpl_results;

#endif /* mpl_results_h */

//
//  mpl_hierarch.h
//  Morphy
//
//  Created by Martin Brazeau on 18/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_hierarch_h
#define mpl_hierarch_h

typedef struct _hierarch mpl_hierarch;

typedef struct _hierarch {
    
    long            num_chars; // Number of characters in the hierarchy
    long            max_chars;
    long*           indices;
    long            nnodes;
    long*           nodes; // Keep a list of relevant absence nodes
    int             nparentstates;
    double*         ddists; // Vector of distances to nearest desc with same state
    double*         adists; // Vector of distances to nearest ancestor with same state
    long            nsubch;
    mpl_hierarch*   subchars;
    
} mpl_hierarch;

#endif /* mpl_hierarch_h */

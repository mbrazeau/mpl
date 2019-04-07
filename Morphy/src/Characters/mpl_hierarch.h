//
//  mpl_hierarch.h
//  Morphy
//
//  Created by Martin Brazeau on 18/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_hierarch_h
#define mpl_hierarch_h

#include "mpl_parsim.h"

typedef struct _hierarch mpl_hierarch;

typedef struct _hierarch {
    
    long            i_parent;
    long            n_subchar;
    long*           subchars;
    mpl_parsdat*    parent_pset;
    int             nsubchr_psets;
    mpl_parsdat**   subchr_psets;
    
} mpl_hierarch;

#endif /* mpl_hierarch_h */

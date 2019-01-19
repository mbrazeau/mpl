//
//  mpl_discchars.h
//  Morphy
//
//  Created by Martin Brazeau on 11/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_discchars_h
#define mpl_discchars_h

#include "mpl_chardefs.h"

typedef struct _discrchars {
    
    long        num_chars;
    long        num_rows;
    long*       orig_indices;
    double*     weights;
    long        start;
    long        end;
    mpl_discr** matrix;
    mpl_discr** actives;
    // Downpass func;
    // Uppass func;
    // Tip func;
    // Downpass2 func;
    // Uppass2 func;
    // Tip2 func;
    // Updater funcs...
    
} mpl_discrchars;


#endif /* mpl_discchars_h */

//
//  mpl_discchars.h
//  Morphy
//
//  Created by Martin Brazeau on 11/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_charbuf_h
#define mpl_charbuf_h

#include "mpl_chardefs.h"


/**
 The mpl_charbuf stores blocks of data of various types. As implemented
 currently, only discrete categorical characters are supported. However, other
 data types, when supported, would be aded here. These structs contain pointers
 to the blocks of discrete data that are used in state set calculations.
 */
typedef struct _charbuf {
    
    long        num_chars;
    long        num_rows;
    long        char_max;
    long        row_max;
    long*       orig_indices;
    double*     weights;
    mpl_data_t  datype;
    mpl_discr** dnset; // The set of downpass characters
    mpl_discr** upset; // The set of uppass characters
    mpl_discr** actives; // Applicable states encountered down to this node
    mpl_discr** tempdn; // Temporary storage of state sets
    mpl_discr** tempup;
    mpl_discr** tempact;
    
} mpl_charbuf;

void mpl_charbuf_init
(const mpl_data_t datype, const long nrows, const long ncols, mpl_charbuf* cb);
void mpl_charbuf_clear(mpl_charbuf* cb);
void mpl_charbuf_add_data_column
(const mpl_discr* datcol, const long colnum, mpl_charbuf* cb);

#endif /* mpl_discchars_h */
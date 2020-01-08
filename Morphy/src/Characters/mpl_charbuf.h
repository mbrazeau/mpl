//
//  mpl_discchars.h
//  Morphy
//
//  Created by Martin Brazeau on 11/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_charbuf_h
#define mpl_charbuf_h

#include <stdbool.h>
#include "mpl_chardefs.h"


typedef struct _charinfo mpl_charinfo;
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
    double*     preweight;
    long*       charchanges;
    long*       minchanges;
    long*       appliccanges;
    long*       nusplits; // Number of unique splits in characters
    long*       n_ndindices;
    long**      indexbufs;
    long**      nodechanges;
    mpl_data_t  datype;
    mpl_discr** dnset; // The set of downpass characters
    mpl_discr** prupset;
    mpl_discr** dnsetf; // second downpass set
    mpl_discr** upset; // The set of uppass characters
    mpl_discr** rtset;
    mpl_discr** actives; // Applicable states encountered down to this node
    mpl_discr** tempdn; // Temporary storage of state sets
    mpl_discr** tempprup;
    mpl_discr** tempdnf; 
    mpl_discr** tempup;
    mpl_discr** tempact;
    
} mpl_charbuf;

void mpl_charbuf_init
(const mpl_data_t datype, const long nrows, const long ncols, mpl_charbuf* cb);
void mpl_charbuf_cleanup(mpl_charbuf* cb);
void mpl_charbuf_add_data_column
(const mpl_discr* datcol, const long colnum, mpl_charinfo* ci, mpl_charbuf* cb);
void mpl_charbuf_store_discr_states(mpl_charbuf* cb);
void mpl_charbuf_restore_discr_states(const long start, const long end, mpl_charbuf* cb);
void mpl_charbuf_fast_restore_discr_states(const long nchar, const long* inds, mpl_charbuf* cb);
int mpl_charbuf_assert_temps_equal_bufs(mpl_charbuf* cb);
long mpl_charbuf_analyze_discr_minchanges(const long i, bool gapmissing, mpl_charbuf* cb);
#endif /* mpl_discchars_h */

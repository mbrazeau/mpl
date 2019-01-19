//
//  mpl_matrix.h
//  Morphy
//
//  Created by Martin Brazeau on 05/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_matrix_h
#define mpl_matrix_h

#include "../mpl_defs.h"
#include "mpl_chardefs.h"
#include "mpl_charinfo.h"

typedef struct _matrix {
    
    long            num_cols;
    long            num_rows;
    mpl_charinfo*   charinfo;
    int             nsymb;
    char*           symbols;
    char*           rawdata;
    char            missingsymb;
    char            gapsymb;
    char            unknownsymb;
    mpl_gap_t       gaphandl;
    
} mpl_matrix;

mpl_matrix* mpl_matrix_new(void);
void        mpl_matrix_delete(mpl_matrix** m);
void        mpl_matrix_set_nrows(long num_rows, mpl_matrix* m);
long        mpl_matrix_get_nrows(mpl_matrix* m);
MPL_RETURN  mpl_matrix_set_ncols(long num_cols, mpl_matrix* m);
long        mpl_matrix_get_ncols(mpl_matrix* m);
MPL_RETURN  mpl_matrix_attach_rawdata(char* rawdat, mpl_matrix* m);
MPL_RETURN  mpl_matrix_detach_rawdata(mpl_matrix* m);

#endif /* mpl_matrix_h */

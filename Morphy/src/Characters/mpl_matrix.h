//
//  mpl_matrix.h
//  Morphy
//
//  Created by Martin Brazeau on 05/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_matrix_h
#define mpl_matrix_h

#include "mpl_chardefs.h"
#include "mpl_charinfo.h"

typedef struct _matrix {
    
    long            num_cols;
    long            num_rows;
    mpl_charinfo*   charinfo;
    char*           rawmatrix;
    
} mpl_matrix;

#endif /* mpl_matrix_h */

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
#include "mpl_charbuf.h"
//#include "mpl_parsim.h"

typedef struct mpl_parsdat mpl_parsdat;

typedef struct _matrix {
    
    long            num_cols;
    long            num_rows;
    long            num_nodes; // The total number of nodes needed.
    mpl_optim_t     optimality;
    mpl_charinfo*   charinfo;
    int             nsymb;
    char*           symbols;
    char*           rawdata;
    char***         rcells; // Raw input cells
    char            missingsymb;
    char            gapsymb;
    char            unknownsymb;
//    double*         weights; // The vector of user-defined weights
//    double**        weightptrs; // A vector of pointers to weights in the data blocks
    mpl_gap_t       gaphandl;
    int             ndatypes; // Number of discrete data types (e.g. fitch, wagner)
    int             datypes[MPL_DATA_T_MAX];
    mpl_charbuf     cbufs[MPL_DATA_T_MAX];
    int             nparsimtypes;
    int             parstypes[MPL_PARSIM_T_MAX];
    int             nasbytype[MPL_PARSIM_T_MAX];
    mpl_parsdat*    parsets;
    int             nparsets;

} mpl_matrix;

mpl_matrix* mpl_matrix_new(void);
void        mpl_matrix_delete(mpl_matrix** m);
MPL_RETURN  mpl_matrix_set_nrows(long num_rows, mpl_matrix* m);
long        mpl_matrix_get_nrows(const mpl_matrix* m);
MPL_RETURN  mpl_matrix_set_ncols(long num_cols, mpl_matrix* m);
long        mpl_matrix_get_ncols(const mpl_matrix* m);
MPL_RETURN  mpl_matrix_set_nnodes(const long num_nodes, mpl_matrix* m);
long        mpl_matrix_get_nnodes(const mpl_matrix* m);
MPL_RETURN  mpl_matrix_attach_rawdata(const char* rawdat, mpl_matrix* m);
MPL_RETURN  mpl_matrix_init(const long nrows, const long ncols, const long nnodes, mpl_matrix* m);
MPL_RETURN  mpl_matrix_detach_rawdata(mpl_matrix* m);
MPL_RETURN  mpl_matrix_set_parsim_t(const long ind, const mpl_parsim_t ptype, mpl_matrix* m);
MPL_RETURN  mpl_matrix_get_parsim_t(mpl_parsim_t* r, const long ind, mpl_matrix* m);
MPL_RETURN  mpl_matrix_set_gap_handle(const mpl_gap_t gaptype, mpl_matrix* m);
MPL_RETURN  mpl_matrix_apply_data(mpl_matrix* m);
MPL_RETURN  mpl_matrix_add_symbol(const char symb, mpl_matrix* m);
MPL_RETURN  mpl_matrix_report(mpl_matrix *m);

#endif /* mpl_matrix_h */

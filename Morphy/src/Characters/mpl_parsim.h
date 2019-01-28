//
//  mpl_parsim.h
//  Morphy
//
//  Created by Martin Brazeau on 23/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_parsim_h
#define mpl_parsim_h

#include <stdbool.h>

#include "../mpl_defs.h"
#include "mpl_charbuf.h"

typedef struct _matrix mpl_matrix;
typedef struct mpl_parsdat mpl_parsdat;

typedef double  (*mpl_dnfxn)(const long left, const long right, long n, mpl_parsdat* pd);
typedef void    (*mpl_upfxn)(const long left, const long right, long n, long anc, mpl_parsdat* pd);
typedef void    (*mpl_branchfxn)(const long n, const long anc, mpl_parsdat* pd);

typedef struct mpl_parsdat {
    long            start;
    long            end;
    long            nchars;
    long*           indexbuf;
    mpl_charbuf*    cbuf; // The buffer to which this subset refers
    mpl_parsim_t    parstype;
    bool            isNAtype;
    mpl_dnfxn       downfxn1;
    mpl_dnfxn       downfxn2;
    mpl_upfxn       upfxn1;
    mpl_upfxn       upfxn2;
    mpl_branchfxn   tipfxn1;
    mpl_branchfxn   rootfxn;
} mpl_parsdat;



void mpl_parsim_assign_stateset_ptrs(mpl_charbuf* cb);

void mpl_parsim_set_range
(const long start, const long end, mpl_parsdat* pd);

void mpl_parsim_set_type
(const mpl_gap_t gaphandl, const mpl_parsim_t ptype, mpl_parsdat* pd);

void mpl_parsim_add_data_column_to_buffer
(mpl_discr* col, mpl_charbuf* cb, mpl_parsdat* pd);

double mpl_fitch_downpass
(const long left, const long right, const long n, mpl_parsdat* pd);

void mpl_fitch_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd);

void mpl_fitch_root(const long n, const long anc, mpl_parsdat* pd);

void mpl_fitch_tip_update(const long tipn, const long anc, mpl_parsdat* pd);

void mpl_fitch_na_tip_update(const long tipn, const long anc, mpl_parsdat* pd);

void mpl_fitch_na_root(const long n, const long anc, mpl_parsdat* pd);

double mpl_fitch_na_first_downpass
(const long left, const long right, const long n, mpl_parsdat* pd);

void mpl_fitch_na_first_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd);

double mpl_fitch_na_second_downpass
(const long left, const long right, const long n, mpl_parsdat* pd);

void mpl_fitch_na_second_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd);

double mpl_parsim_first_downpass
(const long left, const long right, const long n, mpl_matrix* m);
void mpl_parsim_first_uppass
(const long left, const long right, const long n, const long anc, mpl_matrix* m);
double mpl_parsim_second_downpass
(const long left, const long right, const long n, mpl_matrix* m);
void mpl_parsim_second_uppass
(const long left, const long right, const long n, const long anc, mpl_matrix* m);

void mpl_parsim_do_root(const long n, const long anc, mpl_matrix* m);
void mpl_parsim_tip_update(const long n, const long anc, mpl_matrix* m);

#endif /* mpl_parsim_h */

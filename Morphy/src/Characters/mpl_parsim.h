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
#include "mpl_charinfo.h"
#include "mpl_charbuf.h"

typedef struct _matrix mpl_matrix;
typedef struct mpl_parsdat mpl_parsdat;

typedef double  (*mpl_dnfxn)(const long left, const long right, long n, mpl_parsdat* pd);
typedef void    (*mpl_upfxn)(const long left, const long right, long n, long anc, mpl_parsdat* pd);
typedef void    (*mpl_branchfxn)(const long n, const long anc, mpl_parsdat* pd);
typedef double  (*mpl_locfxn)(const double lim, const long src, const long tgt1, const long tgt2, const long troot, mpl_parsdat* pd);

typedef struct mpl_parsdat {
    
    long            start;
    long            end;
    long            nchars;
    long*           nchanges;
    long*           indexbuf;
    double          minscore;
    long*           minchanges;
    int             doeschange; // Number of times a character changes
    double          scorerecall; // For characters needing full-pass estimation
                                 // during a partial reopt.
    double          score; // The score for this parsimony type.
    double          tryscore; // Temporary score for an insertion attempt.
    mpl_charbuf*    cbuf; // The buffer to which this subset refers
    mpl_parsim_t    parstype;
    bool            isNAtype;
    mpl_dnfxn       downfxn1;
    mpl_dnfxn       downfxn2;
    mpl_upfxn       upfxn1;
    mpl_upfxn       upfxn2;
    mpl_branchfxn   tipfxn1;
    mpl_branchfxn   tipfxn2;
    mpl_branchfxn   rootfxn;
    mpl_locfxn      locfxn;
    mpl_dnfxn       srcroot;
    
} mpl_parsdat;

void mpl_parsim_assign_stateset_ptrs(mpl_charbuf* cb);
void mpl_parsim_swap_stateset_ptrs(mpl_charbuf* cb);
void mpl_parsim_set_range(const long start, const long end, mpl_parsdat* pd);
void mpl_parsim_init_parsdat(const long start, const long end, mpl_parsdat* pd);
void mpl_parsim_reset_nchanges(mpl_parsdat* pd);
void mpl_parsim_set_type
(const mpl_gap_t gaphandl, const mpl_parsim_t ptype, mpl_parsdat* pd);
void mpl_parsim_add_data_column_to_buffer
(mpl_discr* col, mpl_charinfo* ci, mpl_charbuf* cb, mpl_parsdat* pd);
double mpl_fitch_downpass
(const long left, const long right, const long n, mpl_parsdat* pd);
void mpl_fitch_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd);
void mpl_fitch_root(const long n, const long anc, mpl_parsdat* pd);
void mpl_fitch_tip_update(const long tipn, const long anc, mpl_parsdat* pd);
void mpl_fitch_na_tip_update(const long tipn, const long anc, mpl_parsdat* pd);
double mpl_fitch_local_check
(const double lim, const long src, const long tgt1, const long tgt2, const long troot, mpl_parsdat* pd);
void mpl_fitch_na_root(const long n, const long anc, mpl_parsdat* pd);
double mpl_do_src_root(const long left, const long right, const long n, mpl_parsdat* pd);
double mpl_na_do_src_root(const long left, const long right, const long n, mpl_parsdat* pd);
double mpl_fitch_na_first_downpass
(const long left, const long right, const long n, mpl_parsdat* pd);
void mpl_fitch_na_first_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd);
double mpl_fitch_na_second_downpass
(const long left, const long right, const long n, mpl_parsdat* pd);
void mpl_fitch_na_root_finalize(const long n, const long anc, mpl_parsdat* pd);
void mpl_fitch_na_second_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd);
void mpl_fitch_na_tip_finalize(const long tipn, const long anc, mpl_parsdat* pd);
double mpl_na_only_parsim_first_downpass
(const long left, const long right, const long n, mpl_matrix* m);
void mpl_na_only_parsim_do_root(const long n, const long anc, mpl_matrix* m);
int mpl_na_only_parsim_first_uppass
(const long left, const long right, const long n, const long anc, mpl_matrix* m);
void mpl_na_only_parsim_tip_update(const long n, const long anc, mpl_matrix* m);
double mpl_na_only_parsim_second_downpass
(const long left, const long right, const long n, mpl_matrix* m);
double mpl_fitch_na_local_check
(const double lim, const long src, const long tgt1, const long tgt2, const long troot, mpl_parsdat* pd);
void mpl_parsim_reset_scores(mpl_matrix* m);
double mpl_parsim_get_std_scores(mpl_matrix* m);
double mpl_parsim_get_na_scores(mpl_matrix* m);
double mpl_parsim_first_downpass
(const long left, const long right, const long n, mpl_matrix* m);
void mpl_parsim_first_uppass
(const long left, const long right, const long n, const long anc, mpl_matrix* m);
double mpl_parsim_second_downpass
(const long left, const long right, const long n, mpl_matrix* m);
void mpl_parsim_second_uppass
(const long left, const long right, const long n, const long anc, mpl_matrix* m);
void mpl_parsim_tip_finalize(const long n, const long anc, mpl_matrix* m);
void mpl_parsim_reset_root_state_buffers(const long n, const long anc, mpl_matrix* m);
void mpl_parsim_do_src_root(const long left, const long right, const long n, mpl_matrix* m);
void mpl_parsim_update_active_sets(const long left, const long right, const long n, mpl_matrix* m);
double mpl_parsim_local_check
(const double lim, const long src, const long tgt1, const long tgt2, const long troot, mpl_matrix* m);
void mpl_parsim_do_root(const long n, const long anc, mpl_matrix* m);
void mpl_parsim_finalize_root(const long n, const long anc, mpl_matrix* m);
void mpl_parsim_tip_update(const long n, const long anc, mpl_matrix* m);
double mpl_parsim_get_score_recall(mpl_matrix* m);
double mpl_parsim_get_standard_tryscore(mpl_matrix* m);
void mpl_reset_state_buffs(const long nrows, mpl_parsdat* pd);
void mpl_parsim_reset_state_buffers(mpl_matrix *m);
double mpl_parsim_get_na_remaining_minscore(mpl_matrix* m);
int mpl_parsim_check_nas_updated(mpl_matrix* m);


void mpl_parsim_temp_set_std(mpl_matrix* m);
void mpl_parsim_temp_reset_std(mpl_matrix* m);

// TODO: remove these when you don't need them anymore
void reset_temporary_changebuf(void);
long get_temp_change(long i);
void tempchangebuf_selective_reset(mpl_matrix *m);
#endif /* mpl_parsim_h */

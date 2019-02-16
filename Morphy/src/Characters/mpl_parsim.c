//
//  mpl_discchars.c
//  Morphy
//
//  Created by Martin Brazeau on 11/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <string.h>
#include <assert.h>

#include "../mpl_utils.h"
#include "mpl_parsim.h"
#include "mpl_matrix.h"



long tempdbgchanges[400] = {0};

void reset_temporary_changebuf(void)
{
    memset(tempdbgchanges, 0, 400 * sizeof(long));
}

long get_temp_change(long i)
{
    return tempdbgchanges[i];
}

void tempchangebuf_selective_reset(mpl_matrix *m)
{
    mpl_parsdat* pd = &m->parsets[1];
    long* indices = pd->indexbuf;
    long nchar = pd->nchars;
    long i = 0;
    long j = 0;
    for (i = 0; i < nchar; ++i) {
        j = indices[i];
        tempdbgchanges[j] = 0;
    }
}


static const mpl_parsdat Fitch_Std = {
    
    .parstype   = MPL_FITCH_T,
    .isNAtype   = false,
    .downfxn1   = &mpl_fitch_downpass,
    .upfxn1     = &mpl_fitch_uppass,
    .downfxn2   = NULL,
    .upfxn2     = NULL,
    .tipfxn1    = &mpl_fitch_tip_update,
    .tipfxn2    = NULL,
    .rootfxn    = &mpl_fitch_root,
    .locfxn     = &mpl_fitch_local_check,
    .srcroot    = &mpl_do_src_root
    
    
};


static const mpl_parsdat Fitch_NA = {
    
    .parstype   = MPL_FITCH_T,
    .isNAtype   = true,
    .downfxn1   = &mpl_fitch_na_first_downpass,
    .upfxn1     = &mpl_fitch_na_first_uppass,
    .downfxn2   = &mpl_fitch_na_second_downpass,
    .upfxn2     = &mpl_fitch_na_second_uppass,
    .tipfxn1    = &mpl_fitch_na_tip_update,
    .tipfxn2    = &mpl_fitch_na_tip_finalize,
    .rootfxn    = &mpl_fitch_na_root,
    .locfxn     = &mpl_fitch_na_local_check,
    .srcroot    = &mpl_na_do_src_root
};


static const mpl_parsdat Wagner_Std = {

    .parstype   = MPL_WAGNER_T,
    .isNAtype   = false,
    
};


// Data from the discrete character charbuf
mpl_discr** restrict dnset   = NULL;
mpl_discr** restrict upset   = NULL;
mpl_discr** restrict actives = NULL;
mpl_discr** restrict tempdn  = NULL;
mpl_discr** restrict tempup  = NULL;
mpl_discr** restrict tempact = NULL;
double*     restrict weights = NULL;
long*       restrict changes = NULL;
long*       restrict minchanges = NULL;


/**
 Initialises global variables in mpl_parsim.c with data from the discrete
 characters buffer.

 @param cb A pointer to an instance of an mpl_charbuf
 */
void mpl_parsim_assign_stateset_ptrs(mpl_charbuf* cb)
{
    dnset   = cb->dnset;
    upset   = cb->upset;
    actives = cb->actives;
    tempdn  = cb->tempdn;
    tempup  = cb->tempup;
    tempact = cb->tempact;
    weights = cb->weights;
    changes = cb->charchanges;
    minchanges = cb->minchanges;
}

void mpl_parsim_swap_stateset_ptrs(mpl_charbuf* cb)
{
    mpl_discr** tup;
    mpl_discr** tdn;
    mpl_discr** tac;
    
    tdn = cb->dnset;
    tup = cb->upset;
    tac = cb->actives;
    
    cb->dnset   = cb->tempdn;
    cb->upset   = cb->tempup;
    cb->actives = cb->tempact;
    
    cb->tempdn  = tdn;
    cb->tempup  = tup;
    cb->tempact = tac;
    
    dnset   = cb->dnset;
    upset   = cb->upset;
    actives = cb->actives;
    tempdn  = cb->tempdn;
    tempup  = cb->tempup;
    tempact = cb->tempact;
}


void mpl_parsim_set_range
(const long start, const long end, mpl_parsdat* pd)
{
    assert(end > start);
    pd->start   = start;
    pd->end     = end;
}


void mpl_parsim_init_parsdat(const long start, const long end, mpl_parsdat* pd)
{
    mpl_parsim_set_range(start, end, pd);
    
    long range = end-start;
    
//    pd->nchanges = (long*)safe_calloc(range, sizeof(long));
    pd->indexbuf = (long*)safe_calloc(range, sizeof(long));
    pd->minchanges = (long*)safe_calloc(range, sizeof(long));
}

//void mpl_parsim_reset_nchanges(mpl_parsdat* pd)
//{
//    long range = pd->end - pd->start;
//
//    memset(pd->nchanges, 0, range * sizeof(long));
//}

void mpl_parsim_reset_indexbuf(mpl_parsdat* pd)
{
    long range = pd->end - pd->start;
    
    memset(pd->indexbuf, 0, range * sizeof(long));
}

void mpl_parsim_set_type
(const mpl_gap_t gaphandl, const mpl_parsim_t ptype, mpl_parsdat* pd)
{
    switch (ptype) {
        case (MPL_FITCH_T):
            if (gaphandl == GAP_INAPPLIC) {
                *pd = Fitch_NA;
            }
            else {
                *pd = Fitch_Std;
            }
            break;
        case (MPL_WAGNER_T):
            if (gaphandl == GAP_INAPPLIC) {
//                *pd = Wagner_NA; // Doesn't exist yet
            }
            else {
                *pd = Wagner_Std; // Doesn't exist yet
            }
            break;
        default:
            break;
    }
}


void mpl_parsim_add_data_column_to_buffer
(mpl_discr* col, mpl_charinfo* ci, mpl_charbuf* cb, mpl_parsdat* pd)
{
    mpl_charbuf_add_data_column(col, pd->start + pd->nchars, ci, cb);
    cb->minchanges[pd->start + pd->nchars] = mpl_charbuf_analyze_discr_minchanges(pd->start + pd->nchars, pd->isNAtype, cb);
    ++pd->nchars;
#ifdef DEBUG
    assert(pd->nchars <= pd->end);
#endif
}

/// Functions to buffer tips and tip indices that would end up requiring
/// updates during the search.

void mpl_parsim_setup_tips(mpl_matrix* m, mpl_parsdat* pd)
{
    long i = 0;
    long j = 0;
    long end = pd->end;
    for (i = 0; i < m->num_rows; ++i) {
        for (j = pd->start; j < end; ++j) {
            if ((dnset[i][j] & (dnset[i][j] - 1)) != 0) {
                // Put this one in the buffer
            }
            else {
                actives[i][j] = tempact[i][j] = tempup[i][j]
                = tempdn[i][j] = upset[i][j] = dnset[i][j];
            }
        }
    }
}

/// Parsimony state set functions

double mpl_fitch_downpass
(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    double cost = 0.0;
    
    for (i = pd->start; i < end; ++i) {
        
//        dnset[n][i] = dnset[left][i] & dnset[right][i];
        
        if (!(dnset[n][i] = dnset[left][i] & dnset[right][i])) {
            dnset[n][i] = dnset[left][i] | dnset[right][i];
            cost += weights[i];
            ++changes[i];
        }
        
//        tempdn[n][i] = dnset[n][i];
    }
    
    pd->score += cost;
    
    return cost;
}


void mpl_fitch_root(const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {
        dnset[anc][i] = upset[anc][i] = dnset[n][i];
    
//        tempdn[anc][i] = tempup[anc][i] = upset[anc][i];
    }
    
    
}


void mpl_fitch_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    mpl_discr fin = 0;
    
    for (i = pd->start; i < end; ++i) {
       
        fin = upset[anc][i] & dnset[n][i];
        
        if (fin != upset[anc][i]) {
            if (dnset[left][i] & dnset[right][i]) {
                fin = (dnset[n][i] | (upset[anc][i] & (dnset[left][i] | dnset[right][i])));
            }
            else {
                fin = dnset[n][i] | upset[anc][i];
            }
        }

        upset[n][i] = fin;
//        tempup[n][i] = fin;
    }
}


void mpl_fitch_tip_update(const long tipn, const long anc, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    mpl_discr t;
    
    for (i = pd->start; i < end; ++i) {
        
        upset[tipn][i] = dnset[tipn][i];
        
//        if (((dnset[tipn][i] - 1) & dnset[tipn][i]) != 0) {
            t = dnset[tipn][i] & upset[anc][i];
            if (t) {
                upset[tipn][i] = t;
            }
            
            tempdn[tipn][i] = dnset[tipn][i];
            tempup[tipn][i] = upset[tipn][i];
//        }
    }
}


double mpl_fitch_local_check
(const double lim,
 const long src,
 const long tgt1,
 const long tgt2,
 const long troot,
 mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    double score = 0.0;
    
    if (lim < 0.0) {
        for (i = pd->start; i < end; ++i) {
            if (!((upset[tgt1][i] | upset[tgt2][i]) & dnset[src][i])) {
                score += weights[i];
            }
        }
    }
    else {
        for (i = pd->start; i < end; ++i) {
            if (!((upset[tgt1][i] | upset[tgt2][i]) & dnset[src][i])) {
                score += weights[i];
                if (score > lim) {
                    return score;
                }
            }
        }
    }
    
    return score;
}


double
mpl_fitch_na_first_downpass
(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i;
    long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {
        
        dnset[n][i] = dnset[left][i] & dnset[right][i];
        
        if (dnset[n][i] == 0) {
            dnset[n][i] = dnset[left][i] | dnset[right][i];
            
            // At this point: could change this to an if-else, and count any
            // steps added here between exclusively applicable characters, then
            // on the else, add the index to a buffer used in the second downpass
            // thereby avoiding the need to call this function over all nodes
            // for all characters again.
            if (dnset[left][i] & ISAPPLIC && dnset[right][i] & ISAPPLIC) {
                dnset[n][i] &= ISAPPLIC;
            }
        }
        else {
            if (dnset[n][i] == NA) {
                if (dnset[left][i] & ISAPPLIC && dnset[right][i] & ISAPPLIC) {
                    dnset[n][i] = dnset[left][i] | dnset[right][i];
                }
            }
        }
        
        tempdn[n][i] = dnset[n][i];
    }
    
    // Only returns a value because it's regular fitch counterpart does but
    // could be changed during program optimisation to get some step changes
    // at this point in the analysis
    return 0.0;
}


void mpl_fitch_na_root(const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {
        upset[anc][i] = dnset[n][i];
        if (upset[anc][i] & ISAPPLIC) {
            upset[anc][i] &= ISAPPLIC;
        }
        
        tempdn[anc][i] = dnset[anc][i];
        tempup[anc][i] = upset[anc][i];
//        assert(upset[anc][i]);
    }
}


void mpl_fitch_na_first_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    mpl_discr t = 0;
    
    for (i = pd->start; i < end; ++i) {
        
        if (dnset[n][i] & NA) {
            if (dnset[n][i] & ISAPPLIC) {
                if (upset[anc][i] == NA) {
                    upset[n][i] = NA;
                } else {
                    upset[n][i] = dnset[n][i] & ISAPPLIC;
                }
            } else {
                if (upset[anc][i] == NA) {
                    upset[n][i] = NA;
                } else {
                    t = (dnset[left][i] | dnset[right][i]) & ISAPPLIC;
                    if (t != 0) {
                        upset[n][i] = t;
                    } else {
                        upset[n][i] = NA;
                    }
                }
            }
        }
        else {
            upset[n][i] = dnset[n][i];
        }
        
//        tempup[n][i] = upset[n][i];
//        assert(upset[n][i]);
    }
}


void mpl_fitch_na_tip_update(const long tipn, const long anc, mpl_parsdat* pd)
{
    long i = 0;
    long end = pd->end;
//    mpl_discr t = 0;
    
    for (i = pd->start; i < end; ++i) {
        
//        if (((dnset[tipn][i] - 1) & dnset[tipn][i]) != 0) {
            if (dnset[tipn][i] & upset[anc][i]) {
                actives[tipn][i] = dnset[tipn][i] & upset[anc][i] & ISAPPLIC;
            } else {
                actives[tipn][i] |= dnset[tipn][i] & ISAPPLIC;
            }
            
            upset[tipn][i] = dnset[tipn][i];
            
            if (dnset[tipn][i] & upset[anc][i]) {
                if (upset[anc][i] & ISAPPLIC) {
                    upset[tipn][i] &= ISAPPLIC;
                }
            }
//        } else {
//            actives[tipn][i] = dnset[tipn][i] & ISAPPLIC;
//        }
        
        tempdn[tipn][i]  = dnset[tipn][i];
        tempup[tipn][i]  = upset[tipn][i];
        tempact[tipn][i] = actives[tipn][i];
    }
}


double mpl_fitch_na_second_downpass
(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i;
    long end = pd->end;
    mpl_discr t = 0;
    double cost = 0.0;
    
    for (i = pd->start; i < end; ++i) {
        if (upset[n][i] & ISAPPLIC) {
            t = upset[left][i] & upset[right][i];
            if (t) {
                if (t & ISAPPLIC) {
                    upset[n][i] = t & ISAPPLIC;
                } else {
                    upset[n][i] = t;
                }
            } else {
                upset[n][i] = (upset[left][i] | upset[right][i]) & ISAPPLIC;
                if (upset[left][i] & ISAPPLIC && upset[right][i] & ISAPPLIC) {
                    cost += weights[i];
                    ++changes[i];
                } else if (actives[left][i] && actives[right][i]) {
                    cost += weights[i];
                    ++changes[i];
                }
            }
        } else {
            if (actives[left][i] && actives[right][i]) {
                cost += weights[i];
                ++changes[i];
            }
        }
//        assert(upset[n][i]);
        actives[n][i] = (actives[left][i] | actives[right][i]) & ISAPPLIC;
        tempact[n][i] = actives[n][i];
    }
    
    pd->score += cost;
    
    return cost;
}


void mpl_fitch_na_root_finalize(const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {
        upset[anc][i] = upset[n][i];
        if (upset[anc][i] & ISAPPLIC) {
            upset[anc][i] &= ISAPPLIC;
        }
        
        tempdn[anc][i] = dnset[anc][i];
        tempup[anc][i] = upset[anc][i];
        //        assert(upset[anc][i]);
    }
}


void mpl_fitch_na_second_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    mpl_discr t = 0;
    for (i = pd->start; i < end; ++i) {
        
        if (upset[n][i] & ISAPPLIC) {
            if (upset[anc][i] & ISAPPLIC) {
               
                t = upset[anc][i] & upset[n][i];
                
                if (t == upset[anc][i]) {
                    
                    upset[n][i] = t;  // B1
                    
                }
                else {
                    if (upset[left][i] & upset[right][i]) {
                        upset[n][i] = (upset[n][i] | (upset[anc][i] & (upset[left][i] | upset[right][i])));// B2
                    } else {
                        if ((upset[left][i] | upset[right][i]) & NA) {
                            if ((upset[anc][i] & (upset[left][i] | upset[right][i]))) {
                                upset[n][i] = upset[anc][i]; // B3
                            }
                            else {
                                upset[n][i] = (upset[anc][i] | upset[left][i] | upset[right][i]) & ISAPPLIC; // B4
                            }
                        } else {
                            upset[n][i] = upset[anc][i] | upset[n][i]; // B5
                        }
                    }
                }
            }
        }
        
        tempup[n][i] = upset[n][i];
//        assert(!(upset[n][i] & NA && upset[n][i] & ISAPPLIC));
//        assert(upset[n][i] & ISAPPLIC || upset[n][i] == NA);
//        assert(upset[n][i]);
    }
}

void mpl_fitch_na_tip_finalize(const long tipn, const long anc, mpl_parsdat* pd)
{
    long i = 0;
    long end = pd->end;

    for (i = pd->start; i < end; ++i) {
        
        if (dnset[tipn][i] & upset[anc][i]) {
            upset[tipn][i] = dnset[tipn][i] & upset[anc][i];
        }
//        else {
//            upset[tipn][i] = dnset[tipn][i];
//        }
    
        tempup[tipn][i] = upset[tipn][i];
    }
}


/// Partial-pass reoptimsation functions

// For inapplicables

double
mpl_fitch_na_recalc_first_downpass
(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i;
    long j;
    long* restrict indices = pd->indexbuf;
    mpl_discr t = 0;
    double chgs = 0.0;
    
    for (j = pd->nchars; j-- ; ) {

        i = indices[j];
        
        t = dnset[left][i] & dnset[right][i];
        
        if (t == 0) {
            t = dnset[left][i] | dnset[right][i];
            if (dnset[left][i] & ISAPPLIC && dnset[right][i] & ISAPPLIC) {
                t &= ISAPPLIC;
            }
        }
        else {
            if (t == NA) {
                if (dnset[left][i] & ISAPPLIC && dnset[right][i] & ISAPPLIC) {
                    t = dnset[left][i] | dnset[right][i];
                }
            }
        }
        
        if (t != tempdn[n][i]) {
            chgs = chgs + 1;
        }
        
        dnset[n][i] = t;
    }
    
    // Only returns a value because it's regular fitch counterpart does but
    // could be changed during program optimisation to get some step changes
    // at this point in the analysis
    return chgs;
}

void mpl_fitch_na_recalc_root(const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    long j;
    
//    for (j = 0; j < pd->nchars; ++j) {
    for (j = pd->nchars; j-- ; ) {
        
        i = pd->indexbuf[j];
        upset[anc][i] = dnset[n][i];
        if (upset[anc][i] & ISAPPLIC) {
            upset[anc][i] &= ISAPPLIC;
        }
    }
}


int mpl_fitch_na_recalc_first_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    long j;
    int chgs = 0;
    mpl_discr t = 0;
    long* restrict indices = pd->indexbuf;
    
//    for (j = 0; j < pd->nchars; ++j) {
    for (j = pd->nchars; j-- ; ) {
        i = indices[j];
        
        t = upset[n][i];
        
        if (dnset[n][i] & NA) {
            if (dnset[n][i] & ISAPPLIC) {
                if (upset[anc][i] == NA) {
                    upset[n][i] = NA;
                } else {
                    upset[n][i] = dnset[n][i] & ISAPPLIC;
                }
            } else {
                if (upset[anc][i] == NA) {
                    upset[n][i] = NA;
                } else {
                    t = (dnset[left][i] | dnset[right][i]) & ISAPPLIC;
                    if (t != 0) {
                        upset[n][i] = t;
                    } else {
                        upset[n][i] = NA;
                    }
                }
            }
        }
        else {
            upset[n][i] = dnset[n][i];
        }

        if (tempup[n][i] != dnset[n][i]) {
            ++chgs;
        }
    }
    
    return chgs;
}

void mpl_fitch_na_recalc_tip_update(const long tipn, const long anc, mpl_parsdat* pd)
{
    long i = 0;
    long j = 0;
//    long end = pd->end;
    //    mpl_discr t = 0;
    long* restrict indices = pd->indexbuf;
    
//    for (j = 0; j < pd->nchars; ++j) {
    for (j = pd->nchars; j-- ; ) {
        
        i = indices[j];

//        if (((dnset[tipn][i] - 1) & dnset[tipn][i]) != 0) {
            if (dnset[tipn][i] & upset[anc][i]) {
                actives[tipn][i] = dnset[tipn][i] & upset[anc][i] & ISAPPLIC;
            } else {
                actives[tipn][i] |= dnset[tipn][i] & ISAPPLIC;
            }
            
            upset[tipn][i] = dnset[tipn][i];
            
            if (dnset[tipn][i] & upset[anc][i]) {
                if (upset[anc][i] & ISAPPLIC) {
                    upset[tipn][i] &= ISAPPLIC;
                }
            }
//        } else {
//            actives[tipn][i] = dnset[tipn][i] & ISAPPLIC;
//        }
//        if (dnset[tipn][i] & upset[anc][i]) {
//            actives[tipn][i] = dnset[tipn][i] & upset[anc][i] & ISAPPLIC;
//        } else {
//            actives[tipn][i] |= dnset[tipn][i] & ISAPPLIC;
//        }
//
//        upset[tipn][i] = dnset[tipn][i];
//
//        if (dnset[tipn][i] & upset[anc][i]) {
//            if (upset[anc][i] & ISAPPLIC) {
//                upset[tipn][i] &= ISAPPLIC;
//            }
//        }
    }
}


double mpl_fitch_na_recalc_second_downpass
(const long left, const long right, const long n, mpl_parsdat* pd)
{
    size_t i;
    size_t j;
    mpl_discr t = 0;
    double cost = 0.0;
    long* restrict indices = pd->indexbuf;
    
    for (j = pd->nchars; j-- ; ) {
        
        i = indices[j];
        
        if (upset[n][i] & ISAPPLIC) {
            t = upset[left][i] & upset[right][i];
            if (t) {
                if (t & ISAPPLIC) {
                    upset[n][i] = t & ISAPPLIC;
                } else {
                    upset[n][i] = t;
                }
            } else {
                upset[n][i] = (upset[left][i] | upset[right][i]) & ISAPPLIC;
                if (upset[left][i] & ISAPPLIC && upset[right][i] & ISAPPLIC) {
                    cost += weights[i];
                } else if (actives[left][i] && actives[right][i]) {
                    cost += weights[i];
                }
            }
        } else {
            if (actives[left][i] && actives[right][i]) {
                cost += weights[i];
            }
        }

        actives[n][i] = (actives[left][i] | actives[right][i]) & ISAPPLIC;
        
        dnset[left][i]    = tempdn[left][i];
        upset[left][i]    = tempup[left][i];
        actives[left][i]  = tempact[left][i];
        dnset[right][i]   = tempdn[right][i];
        upset[right][i]   = tempup[right][i];
        actives[right][i] = tempact[right][i];
        
    }
    
    return cost;
}


/// Local reoptimsation functions for inapplicable characters

double mpl_fitch_na_local_check
(const double lim,
 const long src,
 const long tgt1,
 const long tgt2,
 const long troot,
 mpl_parsdat* pd)
{
    // For any characters that can be checked without direct reopt at this point
    // calculate the length and check if it exceeds the limit. If limit is
    // never hit, then the calling function may need to perform a full check
    // on inapplicable characters.
    
    size_t i;
    const long end = pd->end;
    double score = 0.0;
    
//    if (lim < 0.0) {
        for (i = pd->start; i < end; ++i) {
            
            if (dnset[src][i] & ISAPPLIC) {
                if ((tempup[tgt1][i] | tempup[tgt2][i]) & ISAPPLIC) {
                    if (!((tempup[tgt1][i] | tempup[tgt2][i]) & dnset[src][i])) {
                        score += weights[i];
                    }
                }
                else /*if (dnset[src][i] < MISSING)*/ {
                    pd->indexbuf[pd->nchars] = i;
                    ++pd->nchars;
                    pd->scorerecall += (changes[i] * weights[i]);
                    pd->minscore    += (minchanges[i] * weights[i]);
                }
            } else {
                
                if ((tempup[tgt1][i] | tempup[tgt2][i]) & NA) {
                    
                    if (tempup[tgt1][i] == NA) {
                        if (tempact[tgt1][i] && tempact[src][i]) {
                            score += weights[i];
                        }
//                        else if (tempact[tgt2][i] && tempact[src][i]) {
//                            score += weights[i];
//                        }
                        else /*if (tempact[src][i])*/ {
                            pd->indexbuf[pd->nchars] = i;
                            ++pd->nchars;
                            pd->scorerecall += (changes[i] * weights[i]);
                            pd->minscore    += (minchanges[i] * weights[i]);
                        }
                    }
//                    else if (tempdn[tgt1][i] == NA) {
//                        if (tempact[troot][i] && tempact[src][i]) {
//                            score += weights[i];
//                        }
//                    }
                    else /*if (tempact[src][i] && (tempdn[tgt1][i] | tempdn[tgt2][i]) & ISAPPLIC)*/ {
                        pd->indexbuf[pd->nchars] = i;
                        ++pd->nchars;
                        pd->scorerecall += (changes[i] * weights[i]);
                        pd->minscore    += (minchanges[i] * weights[i]);
                    }
                    
                }
                else /*if (tempact[troot][i])*/ {
                    pd->indexbuf[pd->nchars] = i;
                    ++pd->nchars;
                    pd->scorerecall += (changes[i] * weights[i]);
                    pd->minscore    += (minchanges[i] * weights[i]);
                }
            }
        }
        
        return score;
}


/// External interface functions

void mpl_parsim_do_root(const long n, const long anc, mpl_matrix* m)
{
    int i = 0;
    for (i = 0; i < m->nparsets; ++i) {
        m->parsets[i].rootfxn(n, anc, &m->parsets[i]);
    }
}

void mpl_parsim_finalize_root(const long n, const long anc, mpl_matrix* m)
{
    int i = 0;
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            mpl_fitch_na_root_finalize(n, anc, &m->parsets[i]);
        }
    }
}

void mpl_parsim_reset_scores(mpl_matrix* m)
{
    int i = 0;
    
    for (i = 0; i < m->nparsets; ++i) {
        m->parsets[i].score = 0.0;
    }
    memset(changes, 0, m->cbufs[MPL_DISCR_T].char_max * sizeof(long));
}


double mpl_parsim_get_std_scores(mpl_matrix* m)
{
    int i =  0;
    double ret = 0.0;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == false) {
            ret += m->parsets[i].score;
        }
    }
    
    return ret;
}


double mpl_parsim_get_na_scores(mpl_matrix* m)
{
    int i =  0;
    double ret = 0.0;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            ret += m->parsets[i].score;
        }
    }
    
    return ret;
}


double mpl_parsim_first_downpass
(const long left, const long right, const long n, mpl_matrix* m)
{
    double score = 0.0;
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        score += m->parsets[i].downfxn1(left, right, n, &m->parsets[i]);
    }
    
    return score;
}


void mpl_parsim_tip_update(const long n, const long anc, mpl_matrix* m)
{
    int i = 0;
    for (i = 0; i < m->nparsets; ++i) {
        m->parsets[i].tipfxn1(n, anc, &m->parsets[i]);
    }
}

void mpl_parsim_first_uppass
(const long left, const long right, const long n, const long anc, mpl_matrix* m)
{
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        m->parsets[i].upfxn1(left, right, n, anc, &m->parsets[i]);
    }
}


double mpl_parsim_second_downpass
(const long left, const long right, const long n, mpl_matrix* m)
{
    double score = 0.0;
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            score += m->parsets[i].downfxn2(left, right, n, &m->parsets[i]);
        }
    }
    
    return score;
}


void mpl_parsim_second_uppass
(const long left, const long right, const long n, const long anc, mpl_matrix* m)
{
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            m->parsets[i].upfxn2(left, right, n, anc, &m->parsets[i]);
        }
    }
}

void mpl_parsim_tip_finalize(const long n, const long anc, mpl_matrix* m)
{
    int i = 0;
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
           m->parsets[i].tipfxn2(n, anc, &m->parsets[i]);
        }
        
    }
}

// NA-only passes

double mpl_na_only_parsim_first_downpass
(const long left, const long right, const long n, mpl_matrix* m)
{
    double score = 0.0;
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
//            score += m->parsets[i].downfxn1(left, right, n, &m->parsets[i]);
            score += mpl_fitch_na_recalc_first_downpass(left, right, n, &m->parsets[i]);
        }
    }
    
    return score;
}

void mpl_na_only_parsim_do_root(const long n, const long anc, mpl_matrix* m)
{
    int i = 0;
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            mpl_fitch_na_recalc_root(n, anc, &m->parsets[i]);
        }
    }
}

int mpl_na_only_parsim_first_uppass
(const long left, const long right, const long n, const long anc, mpl_matrix* m)
{
    int i;
    int chgs = 0;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
//            m->parsets[i].upfxn1(left, right, n, anc, &m->parsets[i]);
            chgs += mpl_fitch_na_recalc_first_uppass(left, right, n, anc, &m->parsets[i]);
        }
    }
    
    return chgs;
}


void mpl_na_only_parsim_tip_update(const long n, const long anc, mpl_matrix* m)
{
    int i = 0;
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            mpl_fitch_na_recalc_tip_update(n, anc, &m->parsets[i]);
//           m->parsets[i].tipfxn1(n, anc, &m->parsets[i]);
        }
    }
}

double mpl_na_only_parsim_second_downpass
(const long left, const long right, const long n, mpl_matrix* m)
{
    double score = 0.0;
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            score += mpl_fitch_na_recalc_second_downpass(left, right, n, &m->parsets[i]);
        }
    }
    
    return score;
}

void mpl_reset_root_buffers(const long n, const long anc, mpl_parsdat* pd)
{
    long j = 0;
    long i = 0;
    long* restrict indices = pd->indexbuf;
    
    if (anc != 0) {
        for (j = pd->nchars; j-- ; ) {
            
            i = indices[j];
            
            dnset[n][i]     = tempdn[n][i];
            upset[n][i]     = tempup[n][i];
            actives[n][i]   = tempact[n][i];
            dnset[anc][i]   = tempdn[anc][i];
            upset[anc][i]   = tempup[anc][i];
            actives[anc][i] = tempact[anc][i];
        }
    } else {
        for (j = pd->nchars; j-- ; ) {
            
            i = indices[j];
            
            dnset[n][i]     = tempdn[n][i];
            upset[n][i]     = tempup[n][i];
            actives[n][i]   = tempact[n][i];
        }
    }
}

void mpl_parsim_reset_root_state_buffers(const long n, const long anc, mpl_matrix* m)
{
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            mpl_reset_root_buffers(n, anc, &m->parsets[i]);
        }
    }
}

double mpl_do_src_root(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i = 0;
    long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {
        dnset[n][i] = upset[left][i] | upset[right][i];
    }
    
    return 0.0;
}

double mpl_na_do_src_root(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i = 0;
    long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {
        
        dnset[n][i] = upset[left][i] | upset[right][i];
        
        if (dnset[n][i] & ISAPPLIC) {
            dnset[n][i] &= ISAPPLIC;
        }
        
        tempdn[n][i] = dnset[n][i];
        
        actives[n][i] = (actives[left][i] | actives[right][i]) & ISAPPLIC;
    }
    
    return 0.0;
}

void mpl_parsim_do_src_root(const long left, const long right, const long n, mpl_matrix* m)
{
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        m->parsets[i].srcroot(left, right, n, &m->parsets[i]);
    }
}


void mpl_update_active_sets(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i = 0;
    long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {

        actives[n][i] = (actives[left][i] | actives[right][i]) & ISAPPLIC;
        tempact[n][i] = actives[n][i];
    }
}


void mpl_parsim_update_active_sets(const long left, const long right, const long n, mpl_matrix* m)
{
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            mpl_fitch_na_first_downpass(left, right, n, &m->parsets[i]);
            mpl_update_active_sets(left, right, n, &m->parsets[i]);
        }
    }
}

double mpl_parsim_local_check
(const double lim, const long src, const long tgt1, const long tgt2, const long troot, mpl_matrix* m)
{
    double score = 0.0;
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
//      m->parsets[i].tryscore = 0.0;
        m->parsets[i].scorerecall = 0.0;
        m->parsets[i].nchars = 0;
        m->parsets[i].minscore = 0.0;
        score += m->parsets[i].locfxn(lim, src, tgt1, tgt2, troot, &m->parsets[i]);
//      m->parsets[i].tryscore = score;
    }
    
    return score;
}

double mpl_parsim_get_score_recall(mpl_matrix* m)
{
    double recall = 0.0;
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            recall += m->parsets[i].scorerecall;
        }
    }
    
    return recall;
}

double mpl_parsim_get_standard_tryscore(mpl_matrix* m)
{
    double tryscore = 0.0;
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            tryscore += m->parsets[i].tryscore;
        }
    }
    
    return tryscore;
}

double mpl_parsim_get_na_remaining_minscore(mpl_matrix* m)
{
    int i;
    double minremain = 0.0;

    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            minremain += m->parsets[i].minscore;
        }
    }
    
    return minremain;
}

void mpl_reset_state_buffs(const long nrows, mpl_parsdat* pd)
{
    long i = 0;
    long j = 0;
    long k = 0;
    
    for (i = nrows; i--; ) {
        for (j = pd->nchars; j-- ;) {
            k = pd->indexbuf[j];
            dnset[i][k]     = tempdn[i][k];
            upset[i][k]     = tempup[i][k];
            actives[i][k]   = tempact[i][k];
        }
    }
}

void mpl_parsim_reset_state_buffers(mpl_matrix *m)
{
    int i;

    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            mpl_reset_state_buffs(m->num_nodes, &m->parsets[i]);
        }
    }
}

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

static const mpl_parsdat Fitch_Std = {
    
    .parstype   = MPL_FITCH_T,
    .isNAtype   = false,
    .downfxn1   = &mpl_fitch_downpass,
    .upfxn1     = &mpl_fitch_uppass,
    .downfxn2   = NULL,
    .upfxn2     = NULL,
    .tipfxn1    = &mpl_fitch_tip_update,
    .rootfxn    = &mpl_fitch_root,
    .locfxn     = &mpl_fitch_local_check
    
};


static const mpl_parsdat Fitch_NA = {
    
    .parstype   = MPL_FITCH_T,
    .isNAtype   = true,
    .downfxn1   = &mpl_fitch_na_first_downpass,
    .upfxn1     = &mpl_fitch_na_first_uppass,
    .downfxn2   = &mpl_fitch_na_second_downpass,
    .upfxn2     = &mpl_fitch_na_second_uppass,
    .tipfxn1    = &mpl_fitch_na_tip_update,
    .rootfxn    = &mpl_fitch_na_root,
    .locfxn     = &mpl_fitch_na_local_check
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
    
    pd->nchanges = (long*)safe_calloc(range, sizeof(long));
    pd->indexbuf = (long*)safe_calloc(range, sizeof(long));
}

void mpl_parsim_reset_nchanges(mpl_parsdat* pd)
{
    long range = pd->end - pd->start;
    
    memset(pd->nchanges, 0, range * sizeof(long));
}

void mpl_parsim_reset_indexbuf(mpl_parsdat* pd)
{
    long range = pd->end - pd->start;
    
    memset(pd->nchanges, 0, range * sizeof(long));
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
    ++pd->nchars;
#ifdef DEBUG
    assert(pd->nchars <= pd->end);
#endif
}


/// Parsimony state set functions

double mpl_fitch_downpass
(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    double cost = 0.0;
    
    for (i = pd->start; i < end; ++i) {
        
        dnset[n][i] = dnset[left][i] & dnset[right][i];
        
        if (dnset[n][i] == 0) {
            dnset[n][i] = dnset[left][i] | dnset[right][i];
            cost += weights[i];
            ++pd->nchanges[i];
        }
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
    }
}


void mpl_fitch_tip_update(const long tipn, const long anc, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    mpl_discr t;
    
    for (i = pd->start; i < end; ++i) {
        t = dnset[tipn][i] & upset[anc][i];
        if (t) {
            upset[tipn][i] = t;
        } else {
            upset[tipn][i] = dnset[tipn][i];
        }
    }
}


double mpl_fitch_local_check
(const double lim,
 const long src,
 const long tgt1,
 const long tgt2,
 const mpl_parsdat* pd)
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
        
    }
}


void mpl_fitch_na_tip_update(const long tipn, const long anc, mpl_parsdat* pd)
{
    long i = 0;
    long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {
        
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
//                    ++pd->nchanges[i];
                } else if (actives[left][i] && actives[right][i]) {
                    cost += weights[i];
//                    ++pd->nchanges[i];
                }
            }
        } else {
            if (actives[left][i] && actives[right][i]) {
                cost += weights[i];
//                ++pd->nchanges[i];
            }
        }
        
        actives[n][i] = (actives[left][i] | actives[right][i]) & ISAPPLIC;
    }
    
    pd->score += cost;
    
    return cost;
}


void mpl_fitch_na_second_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    mpl_discr t = 0;
    for (i = pd->start; i < end; ++i) {
        
        if (dnset[n][i] & ISAPPLIC) {
            
            t = dnset[anc][i] & dnset[n][i];
            
            if (t == dnset[anc][i]) {
                dnset[n][i] = t;
            } else {
                if (dnset[left][i] & dnset[right][i]) {
                    dnset[n][i] = (dnset[n][i] | (dnset[anc][i] & (dnset[left][i] | dnset[right][i])));
                } else {
                    if ((dnset[left][i] | dnset[right][i]) & NA) {
                        if ((dnset[anc][i] & (dnset[left][i] | dnset[right][i]))) {
                            dnset[n][i] = dnset[anc][i];
                        }
                        else {
                            dnset[n][i] = (dnset[anc][i] | dnset[left][i] | dnset[right][i]) & ISAPPLIC;
                        }
                    } else {
                        dnset[n][i] = dnset[anc][i] | dnset[n][i];
                    }
                }
            }
        }
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
    
    for (j = 0; j < pd->nchars; ++j) {
        
        i = pd->indexbuf[j];
        
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
    }
    
    // Only returns a value because it's regular fitch counterpart does but
    // could be changed during program optimisation to get some step changes
    // at this point in the analysis
    return 0.0;
}

void mpl_fitch_na_recalc_first_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    long j;
    mpl_discr t = 0;
    
    for (j = 0; j < pd->nchars; ++j) {
        
        i = pd->indexbuf[j];
        
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
        
    }
}

double mpl_fitch_na_recalc_second_downpass
(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i;
    long j;
    mpl_discr t = 0;
    double cost = 0.0;
    
    for (j = 0; j < pd->nchars; ++j) {
        
        i = pd->indexbuf[j];
        
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
    }
    
    pd->score += cost;
    
    return cost;
}


/// Local reoptimsation functions for inapplicable characters

double mpl_fitch_na_local_check
(const double lim,
 const long src,
 const long tgt1,
 const long tgt2,
 mpl_parsdat* pd)
{
    // For any characters that can be checked without direct reopt at this point
    // calculate the length and check if it exceeds the limit. If limit is
    // never hit, then the calling function may need to perform a full check
    // on inapplicable characters.
    
    long i;
    const long end = pd->end;
    double score = 0.0;
    
    if (lim < 0.0) {
        for (i = pd->start; i < end; ++i) {
            if ((upset[tgt1][i] | upset[tgt2][i]) & ISAPPLIC
                && upset[src][i] & ISAPPLIC) {
                if (!((upset[tgt1][i] | upset[tgt2][i]) & upset[src][i])) {
                    score += weights[i];
                }
            }
            else if (upset[tgt1][i] & upset[src][i] & NA) {
                if (actives[tgt1][i] && actives[src][i]) {
                    score += weights[i];
                }
            }
            else {
                // Add this index to the buffer needing checks
                pd->indexbuf[pd->nchars] = i;
                ++pd->nchars;
                // And record its old score
                pd->scorerecall += (pd->nchanges[i] * weights[i]);
            }
        }
        
        return score;
    }
    else {
        for (i = pd->start; i < end; ++i) {
            if ((upset[tgt1][i] | upset[tgt2][i]) & ISAPPLIC
                && upset[src][i] & ISAPPLIC) {
                if (!((upset[tgt1][i] | upset[tgt2][i]) & upset[src][i])) {
                    score += weights[i];
                    if (score > lim) {
                        return score;
                    }
                }
            }
            else if (upset[tgt1][i] & upset[src][i] & NA) {
                if (actives[tgt1][i] && actives[src][i]) {
                    score += weights[i];
                    if (score > lim) {
                        return score;
                    }
                }
            }
            else {
                // Add this index to the buffer needing checks
                pd->indexbuf[pd->nchars] = i;
                ++pd->nchars;
                // And record its old score
                pd->scorerecall += (pd->nchanges[i] * weights[i]);
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


void mpl_parsim_reset_scores(mpl_matrix* m)
{
    int i = 0;
    
    for (i = 0; i < m->nparsets; ++i) {
        m->parsets[i].score = 0.0;
    }
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

// NA-only passes

double mpl_na_only_parsim_first_downpass
(const long left, const long right, const long n, mpl_matrix* m)
{
    double score = 0.0;
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            score += m->parsets[i].downfxn1(left, right, n, &m->parsets[i]);
        }
    }
    
    return score;
}


void mpl_na_only_parsim_first_uppass
(const long left, const long right, const long n, const long anc, mpl_matrix* m)
{
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            m->parsets[i].upfxn1(left, right, n, anc, &m->parsets[i]);
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
            score += m->parsets[i].downfxn2(left, right, n, &m->parsets[i]);
        }
    }
    
    return score;
}


//void mpl_na_only_parsim_second_uppass
//(const long left, const long right, const long n, const long anc, mpl_matrix* m)
//{
//    int i;
//
//    for (i = 0; i < m->nparsets; ++i) {
//        if (m->parsets[i].isNAtype == true) {
//            m->parsets[i].upfxn2(left, right, n, anc, &m->parsets[i]);
//        }
//    }
//}

double mpl_parsim_local_check
(const double lim, const long src, const long tgt1, const long tgt2, mpl_matrix* m)
{
    double score = 0.0;
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        
        // TODO: TEMPORARY CONDITION:
        if (m->parsets[i].isNAtype == false) {
        
            m->parsets[i].tryscore = 0.0;
            // Reset the number of indices in the index buffer to 0
            m->parsets[i].scorerecall = 0.0;
            m->parsets[i].nchars = 0;
            score += m->parsets[i].locfxn(lim, src, tgt1, tgt2, &m->parsets[i]);
            m->parsets[i].tryscore = score;
        
        }
    }
    
    return score;
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

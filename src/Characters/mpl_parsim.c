//
//  mpl_discchars.c
//  Morphy
//
//  Created by Martin Brazeau on 11/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include <string.h>
#include <assert.h>
#include <limits.h>

#include "mpl_matrix.h"
#include "mpl_parsim.h"

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

// Settings for parsimony types

static const mpl_parsdat Fitch_Std = {
    .parstype   = MPL_FITCH_T,
    .isNAtype   = false,
    .downfxn1   = &mpl_fitch_downpass,
    .upfxn1     = &mpl_fitch_uppass,
    .downfxn2   = NULL,
    .upfxn2     = NULL,
    .tipfxn1    = &mpl_tip_update,
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
    .rrupdate   = &mpl_fitch_na_down_reroot,
    .rednfxn2   = &mpl_fitch_na_recalc_second_downpass,
    .upfxn2     = &mpl_fitch_na_second_uppass,
    .tipfxn1    = &mpl_na_tip_update,
    .tipfxn2    = &mpl_na_tip_finalize,
    .rootfxn    = &mpl_fitch_na_root,
    .locfxn     = &mpl_fitch_na_local_check,
    .srcroot    = &mpl_na_do_src_root
};

static const mpl_parsdat Wagner_Std = {
    .parstype   = MPL_WAGNER_T,
    .isNAtype   = false,
    .downfxn1   = &mpl_wagner_downpass,
    .upfxn1     = &mpl_wagner_uppass,
    .downfxn2   = NULL,
    .upfxn2     = NULL,
    .tipfxn1    = &mpl_tip_update,
    .tipfxn2    = NULL,
    .rootfxn    = &mpl_fitch_root,
    .locfxn     = &mpl_wagner_local_check,
    .srcroot    = &mpl_wagner_src_root
};

static const mpl_parsdat Wagner_NA = {
    .parstype   = MPL_WAGNER_T,
    .isNAtype   = true,
    .downfxn1   = &mpl_fitch_na_first_downpass,
    .upfxn1     = &mpl_fitch_na_first_uppass,
    .downfxn2   = &mpl_wagner_na_second_downpass,
    .rrupdate   = &mpl_wagner_na_down_reroot,
    .upfxn2     = &mpl_wagner_na_second_uppass,
    .rednfxn2   = &mpl_wagner_na_recalc_second_downpass,
    .tipfxn1    = &mpl_na_tip_update,
    .tipfxn2    = &mpl_na_tip_finalize,
    .rootfxn    = &mpl_fitch_na_root,
    .locfxn     = &mpl_wagner_na_local_check,
    .srcroot    = &mpl_wagner_na_do_src_root
};

// Data from the discrete character charbuf
mpl_discr**  dnset          = NULL;
mpl_discr**  prupset        = NULL;
mpl_discr**  dnsetf         = NULL;
mpl_discr**  upset          = NULL;
mpl_discr**  rtset          = NULL;
mpl_discr**  actives        = NULL;
mpl_discr**  tempdn         = NULL;
mpl_discr**  tempprup       = NULL;
mpl_discr**  tempdnf        = NULL;
mpl_discr**  tempup         = NULL;
mpl_discr**  tempact        = NULL;
mpl_discr*   actmask        = NULL;
double*      weights        = NULL;
double*      preweight      = NULL;
short**      regdist        = NULL;
long*        changes        = NULL;
long*        applicchgs     = NULL;
long*        nusplits       = NULL;
long*        minchanges     = NULL;
long*        n_ndindices    = NULL;
long**       indexbufs      = NULL;
long**       nodechanges    = NULL;


/**
 Initialises global variables in mpl_parsim.c with data from the discrete
 characters buffer.

 @param cb A pointer to an instance of an mpl_charbuf
 */
void mpl_parsim_assign_stateset_ptrs(mpl_charbuf* cb)
{
    dnset           = cb->dnset;
    prupset         = cb->prupset;
    dnsetf          = cb->dnsetf;
    upset           = cb->upset;
    rtset           = cb->rtset;
    actives         = cb->actives;
    tempdn          = cb->tempdn;
    tempdnf         = cb->tempdnf;
    tempprup        = cb->tempprup;
    tempup          = cb->tempup;
    tempact         = cb->tempact;
    actmask         = cb->actmask;
    weights         = cb->weights;
    preweight       = cb->preweight;
    changes         = cb->charchanges;
    applicchgs      = cb->appliccanges;
    nusplits        = cb->nusplits;
    minchanges      = cb->minchanges;
    n_ndindices     = cb->n_ndindices;
    indexbufs       = cb->indexbufs;
    nodechanges     = cb->nodechanges;
    regdist         = cb->regdist;
}

void mpl_parsim_do_ratchet_weights(const int factor, mpl_charbuf* cb)
{
    
    long i = 0;
    
    // Take 10% of characters and double their weights
    for (i = 0; i < cb->num_chars; ++i) {
        if (preweight[i] > ((double)mpl_rng()/MPL_RAND_MAX)) {
            weights[i] *= factor;
        }
    }
}

void mpl_parsim_reset_all_weights(mpl_charbuf* cb)
{
    long i = 0;
    for (i = 0; i < cb->num_chars; ++i) {
        weights[i] = DEFAULT_WEIGHT; // TODO: This can't be forever
    }
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
    pd->indexbuf   = (long*)safe_calloc(range, sizeof(long));
    pd->rindexbuf  = (long*)safe_calloc(range, sizeof(long));
    pd->minchanges = (long*)safe_calloc(range, sizeof(long));
}

void mpl_parsim_setup_nodal_index_buffers(const long nnodes, mpl_parsdat* pd)
{
    long i = 0;
    
    pd->nnodes = nnodes;
    pd->nndindices = (long*)safe_calloc(nnodes, sizeof(long));
    pd->ndindexbufs = (long**)safe_calloc(nnodes, sizeof(long*));
    
    for (i = 0; i < pd->nnodes; ++i) {
        // TODO: This isn't fully safe because this function _could_ be called before end and start are specified
        pd->ndindexbufs[i] = (long*)safe_calloc(pd->end - pd->start, sizeof(long));
    }
}

void mpl_parsim_cleanup_parsdat(mpl_parsdat* pd)
{
    long i = 0;
    
//    safe_free(pd->nchanges);
    safe_free(pd->indexbuf);
    safe_free(pd->rindexbuf);
    safe_free(pd->ntipinbufs);
    safe_free(pd->minchanges);
    
    for (i = 0; i < pd->ntips; ++i) {
        safe_free(pd->tipinbufs[i]);
    }
    
    for (i = 0; i < pd->nnodes; ++i) {
        safe_free(pd->ndindexbufs[i]);
    }
    safe_free(pd->ndindexbufs);
    safe_free(pd->nndindices);
    
    safe_free(pd->tipinbufs);
    safe_free(pd->ntipinbufs);
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
                *pd = Wagner_NA; // TODO: Doesn't exist yet
            }
            else {
                *pd = Wagner_Std; // TODO: Doesn't exist yet
            }
            break;
        default:
            break;
    }
}


void mpl_parsim_analyse_informative
 (mpl_discr* col, mpl_charinfo* ci, mpl_charbuf* cb)
{
    int i = 0;
    int j = 0;
    mpl_discr a = NOCHARS;
    
    ci->isvariable = false;
    ci->isparsinform = false;
    
    int nstates = 0;
    
    for (i = 0; i < cb->num_rows; ++i) {
        if (col[i] < UNKNOWN && ((col[i] & (col[i] - 1)) == 0)) {
        
            if (col[i] != (col[i] & a)) {
                a |= col[i];
                ++nstates;
            }
        }
    }

    if (nstates == 0) {
        return;
    }

    int* statecounts = safe_calloc(nstates, sizeof(int));
    mpl_discr* states = safe_calloc(nstates, sizeof(mpl_discr));
    
    nstates = 0;
    
    a = NOCHARS;

    for (i = 0; i < cb->num_rows; ++i) {
        
        if (col[i] < UNKNOWN && ((col[i] & (col[i] - 1)) == 0)) {
            
            if (col[i] != (col[i] & a)) {
                a |= col[i];
                states[nstates] = col[i];
                ++statecounts[nstates];
                ++nstates;
            } else {
                // Search for the state
                for (j = 0; j < nstates; ++j) {
                    if (col[i] == states[j]) {
                        ++statecounts[j];
                    }
                }
            }
        }
    }
    
    for (i = 0; i < nstates-1; ++i) {
        if (statecounts[i] > 1) {
            for (j = i+1; j < nstates; ++j) {
                if (statecounts[j] > 1) {
                    ci->isparsinform = true;
                    break;
                }
            }
            if (ci->isparsinform == true) {
                break;
            }
        }
    }
    
    free(states);
    free(statecounts);
}

void mpl_parsim_add_data_column_to_buffer
(mpl_discr* col, mpl_charinfo* ci, mpl_charbuf* cb, mpl_parsdat* pd)
{
    
    // Write the character in
    mpl_charbuf_add_data_column(col, pd->start + pd->nchars, ci, cb);
    
    // Analyse minimum number of changes
    cb->minchanges[pd->start + pd->nchars] = mpl_charbuf_analyze_discr_minchanges(pd->start + pd->nchars, pd->isNAtype, cb);
    ++pd->nchars;
    
    // Analyse informative characters
    mpl_parsim_analyse_informative(col, ci, cb);
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
    
    if (pd->ntipinbufs == NULL) {
        pd->ntipinbufs = (long*)safe_calloc(m->num_rows, sizeof(long));
    }
    
    if (pd->tipinbufs == NULL) {
        pd->tipinbufs  = (long**)safe_calloc(m->num_rows, sizeof(long*));
        for (i = 0; i < m->num_rows; ++i) {
            pd->tipinbufs[i] = (long*)safe_calloc(m->num_cols, sizeof(long));
        }
        
    }

    pd->ntips = m->num_rows;
    
    for (i = 0; i < m->num_rows; ++i) {
    
        pd->ntipinbufs[i] = 0;
        
        for (j = pd->start; j < end; ++j) {
            
            if ((dnset[i][j] & (dnset[i][j] - 1)) != 0) {
                // Put this one in the buffer
                pd->tipinbufs[i][pd->ntipinbufs[i]] = j;
                ++pd->ntipinbufs[i];
            }
            else {
                tempup[i][j] = tempdn[i][j] = upset[i][j] = dnset[i][j];
                prupset[i][j] = tempprup[i][j] = dnset[i][j];
                actives[i][j] = tempact[i][j] = (dnset[i][j] & ISAPPLIC);
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
        
        dnset[n][i] = dnset[left][i] & dnset[right][i];
        
        if (!dnset[n][i]) {
            dnset[n][i] = dnset[left][i] | dnset[right][i];
            cost += weights[i];
            ++changes[i];
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
        rtset[anc][i] = upset[n][i] | upset[anc][i];
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
        rtset[n][i] = upset[n][i] | upset[anc][i];
//        assert(upset[n][i] != 0);
//        tempup[n][i] = fin;
    }
}


void mpl_tip_update(const long tipn, const long anc, mpl_parsdat* pd)
{
    long i = 0;
    const long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {
        
        upset[tipn][i] = dnset[tipn][i];

        if ((dnset[tipn][i] & upset[anc][i]) == upset[anc][i]) {
            upset[tipn][i] = dnset[tipn][i] & upset[anc][i];
        } /*else if (pd->parstype == MPL_WAGNER_T) {
            if (dnset[tipn][i] & upset[anc][i]) {
                upset[tipn][i] = dnset[tipn][i] & upset[anc][i];
            }
        }*/
        
//        if (pd->parstype == MPL_WAGNER_T && !(upset[tipn][i] & upset[anc][i])) {
//            mpl_smallest_closed_interval(&rtset[tipn][i], upset[tipn][i], upset[anc][i]);
//            rtset[tipn][i] |= upset[tipn][i] | upset[anc][i];
//        } else {
            rtset[tipn][i] = upset[tipn][i] | upset[anc][i];
//        }
        
        
//        tempdn[tipn][i] = dnset[tipn][i];
//        tempup[tipn][i] = upset[tipn][i];
    }
}

#ifdef DEBUG
long localchgs[800] = {0};

void clear_local_changes(void)
{
    memset(localchgs, 0, 800 * sizeof(long));
}

void add_local_changes(mpl_charbuf* cb)
{
    size_t i = 0;
    for (i = 0; i < cb->num_chars; ++i) {
        changes[i] += localchgs[i];
    }
}

#endif

double mpl_fitch_local_check
(const double lim,
 const double base,
 const long src,
 const long tgt1,
 const long tgt2,
 const long troot,
 mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    double score = 0.0;

#ifdef DEBUG
    clear_local_changes();
#endif
    
//    if (lim < 0.0) {
        for (i = pd->start; i < end; ++i) {
            if (!(rtset[tgt1][i] & dnset[src][i])) {
                score += weights[i];
#ifdef DEBUG
                localchgs[i] += weights[i];
#endif
            }
        }
//    }
//    else {
//        for (i = pd->start; i < end; ++i) {
//            if (!((upset[tgt1][i] | upset[tgt2][i]) & dnset[src][i])) {
//                score += weights[i];
//                if (score > lim) {
//                    return score;
//                }
//            }
//        }
//    }
    
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
        
        prupset[anc][i] = upset[anc][i];
        
        tempdn[anc][i] = dnset[anc][i];
        tempprup[anc][i] = prupset[anc][i];
//        tempup[anc][i] = upset[anc][i];
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
                if (prupset[anc][i] == NA) {
                    prupset[n][i] = NA;
                } else {
                    prupset[n][i] = dnset[n][i] & ISAPPLIC;
                }
            } else {
                if (prupset[anc][i] == NA) {
                    prupset[n][i] = NA;
                } else {
                    t = (dnset[left][i] | dnset[right][i]) & ISAPPLIC;
                    if (t != 0) {
                        prupset[n][i] = t;
                    } else {
                        prupset[n][i] = NA;
                    }
                }
            }
        }
        else {
            prupset[n][i] = dnset[n][i];
        }
        
        tempprup[n][i] = prupset[n][i];

    }
}


void mpl_na_tip_update(const long tipn, const long anc, mpl_parsdat* pd)
{
    long i = 0;
    long end = pd->end;
//    mpl_discr t = 0;
    
    for (i = pd->start; i < end; ++i) {

        if (dnset[tipn][i] & prupset[anc][i]) {
            actives[tipn][i] = dnset[tipn][i] & prupset[anc][i] & actmask[i];
        } else {
            actives[tipn][i] |= dnset[tipn][i] & actmask[i];
        }
        
        prupset[tipn][i] = dnset[tipn][i];
        
        if (dnset[tipn][i] & prupset[anc][i]) {
            if (prupset[anc][i] & ISAPPLIC) {
                prupset[tipn][i] &= ISAPPLIC;
            }
        }
        
        dnsetf[tipn][i] = prupset[tipn][i];
        
//        tempdn[tipn][i]   = dnset[tipn][i];
        tempdnf[tipn][i]  = dnsetf[tipn][i];
        tempprup[tipn][i] = prupset[tipn][i];
        tempact[tipn][i]  = actives[tipn][i];
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
        
        nodechanges[n][i] = 0L;
        
        if (prupset[n][i] & ISAPPLIC) {
            t = dnsetf[left][i] & dnsetf[right][i];
            if (t) {
                if (t & ISAPPLIC) {
                    dnsetf[n][i] = t & ISAPPLIC;
                } else {
                    dnsetf[n][i] = t;
                }
            } else {
                dnsetf[n][i] = (dnsetf[left][i] | dnsetf[right][i]) & ISAPPLIC;
                if (dnsetf[left][i] & ISAPPLIC && dnsetf[right][i] & ISAPPLIC) {
                    cost += weights[i];
                    ++changes[i];
                    ++applicchgs[i];
                    nodechanges[n][i] = 1L;
                } else if (actives[left][i] && actives[right][i]) {
                    cost += weights[i];
                    ++changes[i];
                    nodechanges[n][i] = 1L;
                }
            }
        } else {
            dnsetf[n][i] = prupset[n][i];
            if (actives[left][i] && actives[right][i]) {
                cost += weights[i];
                ++changes[i];
                ++nusplits[i];
                nodechanges[n][i] = 1L;
            }
        }
//        assert(upset[n][i]);
        actives[n][i] = (actives[left][i] | actives[right][i]) & ISAPPLIC;
        
        tempact[n][i] = actives[n][i];
        tempdnf[n][i] = dnsetf[n][i];
    }
    
    pd->score += cost;
    
    return cost;
}

double mpl_fitch_na_down_reroot
(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i;
    long end = pd->end;
    mpl_discr t = 0;
    double cost = 0.0;
    
    for (i = pd->start; i < end; ++i) {
        
        nodechanges[n][i] = 0L;
        
        if (prupset[n][i] & ISAPPLIC) {
            
            t = dnsetf[left][i] & dnsetf[right][i];
            
            if (t) {
                if (t & ISAPPLIC) {
                    dnsetf[n][i] = t & ISAPPLIC;
                } else {
                    dnsetf[n][i] = t;
                }
            } else {
                dnsetf[n][i] = (dnsetf[left][i] | dnsetf[right][i]) & ISAPPLIC;
                if (dnsetf[left][i] & ISAPPLIC && dnsetf[right][i] & ISAPPLIC) {
                    nodechanges[n][i] = 1L;
                } else if (actives[left][i] && actives[right][i]) {
                    nodechanges[n][i] = 1L;
                }
            }
        } else {
            dnsetf[n][i] = prupset[n][i];
            if (actives[left][i] && actives[right][i]) {
                nodechanges[n][i] = 1L;
            }
        }
        
//        assert(upset[n][i]);
        actives[n][i] = (actives[left][i] | actives[right][i]) & ISAPPLIC;
        tempact[n][i] = actives[n][i];
        tempdnf[n][i] = dnsetf[n][i];
    }
    
//    pd->score += cost;
    
    return cost;
}

void mpl_fitch_na_root_finalize(const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {
        
        upset[anc][i] = dnsetf[n][i];
        dnsetf[anc][i] = dnsetf[n][i];
        
//        tempdn[anc][i] = dnset[anc][i];
        tempdnf[anc][i] = dnsetf[anc][i];
        tempup[anc][i] = upset[anc][i];
        
        rtset[n][i] = upset[n][i] | upset[anc][i];
    }
}


void mpl_fitch_na_second_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    mpl_discr t = 0;
    for (i = pd->start; i < end; ++i) {
        
        if (dnsetf[n][i] & ISAPPLIC) {
            if (upset[anc][i] & ISAPPLIC) {
                t = upset[anc][i] & dnsetf[n][i];
                if (t == upset[anc][i]) {
                    upset[n][i] = t;  // B1
                }
                else {
                    if (dnsetf[left][i] & dnsetf[right][i]) {
                        upset[n][i] = (dnsetf[n][i] | (upset[anc][i] & (dnsetf[left][i] | dnsetf[right][i])));// B2
                    } else {
                        if ((dnsetf[left][i] | dnsetf[right][i]) & NA) {
                            if ((upset[anc][i] & (dnsetf[left][i] | dnsetf[right][i]))) {
                                upset[n][i] = upset[anc][i]; // B3
                            }
                            else {
                                upset[n][i] = (upset[anc][i] | dnsetf[left][i] | dnsetf[right][i]) & ISAPPLIC; // B4
                            }
                        } else {
                            upset[n][i] = upset[anc][i] | dnsetf[n][i]; // B5
                        }
                    }
                }
            }
            else {
                upset[n][i] = dnsetf[n][i];
            }
        }
        else {
            upset[n][i] = dnsetf[n][i];
            if (upset[anc][i] & ISAPPLIC) {
                regdist[n][i] = 1;
            } else {
                regdist[n][i] = regdist[anc][i] + 1;
            }
        }
        
        rtset[n][i] = upset[n][i] | upset[anc][i];
        
        tempup[n][i] = upset[n][i];
//        assert(!(upset[n][i] & NA && upset[n][i] & ISAPPLIC));
//        assert(upset[n][i] & ISAPPLIC || upset[n][i] == NA);
//        assert(upset[n][i]);
    }
}

void mpl_na_tip_finalize(const long tipn, const long anc, mpl_parsdat* pd)
{
    long i = 0;
    long end = pd->end;

    for (i = pd->start; i < end; ++i) {
        
        if ((dnset[tipn][i] & upset[anc][i]) == upset[anc][i]) {
            upset[tipn][i] = dnset[tipn][i] & upset[anc][i];
        }
        else {
            upset[tipn][i] = dnset[tipn][i];
        }
        
//        if (pd->parstype == MPL_WAGNER_T) {
//            if (dnset[tipn][i] & upset[anc][i]) {
//                upset[tipn][i] = dnset[tipn][i] & upset[anc][i];
//            }
//        }
    
        tempup[tipn][i] = upset[tipn][i];
    
//        if (pd->parstype == MPL_WAGNER_T && !(upset[tipn][i] & upset[anc][i])) {
//            if (!((upset[tipn][i] | upset[anc][i]) & NA)) {
//                mpl_smallest_closed_interval(&rtset[tipn][i], upset[tipn][i], upset[anc][i]);
//            } else {
//                rtset[tipn][i] = NOCHARS; // Clear it so self-assignment in next line doesn't add old states
//            }
//            rtset[tipn][i] |= upset[tipn][i] | upset[anc][i];
//        } else {
            rtset[tipn][i] = upset[tipn][i] | upset[anc][i];
//        }
    }
}


/// Partial-pass reoptimsation functions

// For inapplicables

double
mpl_fitch_na_recalc_first_downpass
(const long left, const long right, const long n, mpl_parsdat*  pd)
{
    long i;
    long j;
    long*  indices = pd->indexbuf;
    mpl_discr t = 0;
    double chgs = 0.0;
    
    for (j = 0; j < pd->nchars; ++j) {

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
        
        if (t != tempdn[n][i]/* || t == NA*/) {
//             if (!((t & tempdn[n][i]) & NA)) {
              ++chgs;
//             }
        }
        
        dnset[n][i] = t;
    }
    
    return chgs;
}

void mpl_fitch_na_recalc_root(const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    long j;
    
//    for (j = 0; j < pd->nchars; ++j) {
    for (j = 0; j < pd->nchars; ++j) {
        
        i = pd->indexbuf[j];
        prupset[anc][i] = dnset[n][i];
//        if (prupset[anc][i] & ISAPPLIC) {
//            prupset[anc][i] &= ISAPPLIC;
//        }
    }
}


int mpl_fitch_na_recalc_first_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    long j;
    int chgs = 0;
    mpl_discr t = 0;
    long*  indices = pd->indexbuf;
    
    pd->nndindices[n] = 0;
    
    for (j = 0; j < pd->nchars;  ++j) {
        
        i = indices[j];
        
        t = 0;
        
        if (dnset[n][i] & NA) {
            if (dnset[n][i] & ISAPPLIC) {
                if (prupset[anc][i] == NA) {
                    prupset[n][i] = NA;
                } else {
                    prupset[n][i] = dnset[n][i] & ISAPPLIC;
                }
            } else {
                if (prupset[anc][i] == NA) {
                    prupset[n][i] = NA;
                } else {
                    t = (dnset[left][i] | dnset[right][i]) & ISAPPLIC;
                    if (t != 0) {
                        prupset[n][i] = t;
                    } else {
                        prupset[n][i] = NA;
                    }
                }
            }
        }
        else {
            prupset[n][i] = dnset[n][i];
        }

        if (tempprup[n][i] != prupset[n][i]) {
            if (tempprup[n][i] == NA || prupset[n][i] == NA) {
                ++chgs;
                pd->ndindexbufs[n][pd->nndindices[n]] = i;
                ++pd->nndindices[n];
            }
        }
    }
    
    return chgs;
}

void mpl_na_recalc_tip_update(const long tipn, const long anc, mpl_parsdat* pd)
{
    size_t i = 0;
    size_t j = 0;
//    size_t k = 0;
//    long r = 0;
    
    long*  indices = pd->indexbuf;
    
    for (j = 0; j < pd->nchars; ++j) {
        
        i = indices[j];
        
        if (dnset[tipn][i] & prupset[anc][i]) {
            actives[tipn][i] = dnset[tipn][i] & prupset[anc][i] & actmask[i];
        } else {
            actives[tipn][i] |= dnset[tipn][i] & actmask[i];
        }
        
        prupset[tipn][i] = dnset[tipn][i];
        
        if (dnset[tipn][i] & prupset[anc][i]) {
            if (prupset[anc][i] & ISAPPLIC) {
                prupset[tipn][i] &= ISAPPLIC;
            }
        }
        
        dnsetf[tipn][i] = prupset[tipn][i];
    }
}


double mpl_fitch_na_recalc_second_downpass
(const long left, const long right, const long n, mpl_parsdat*  pd)
{
    size_t i;
    size_t j;
    long end = 0;
    register double cost = 0.0;
    
    long*  indices = pd->indexbuf;
    end = pd->nchars;
    
    // If possible, limit reconstructions to characters updated at this node.
    if (pd->usndidx == true) {
        indices = pd->ndindexbufs[n];
        end = pd->nndindices[n];
    }
    
    for (j = 0; j < end; ++j) {
        
        i = indices[j];

        // Deduct the old score added at this node.
        cost -= (nodechanges[n][i] * weights[i]);

        // More efficient implementation?
        if (prupset[n][i] & ISAPPLIC) {
            dnsetf[n][i] = (dnsetf[left][i] & dnsetf[right][i]) & ISAPPLIC;
            if (!dnsetf[n][i]) {
                dnsetf[n][i] = (dnsetf[left][i] | dnsetf[right][i]) & ISAPPLIC;

                if (dnsetf[left][i] & ISAPPLIC && dnsetf[right][i] & ISAPPLIC) {
                    cost += weights[i];
                }else if (actives[left][i] && actives[right][i]) {
                    cost += weights[i];
                }
            }
        } else {
            dnsetf[n][i] = prupset[n][i];
            if (actives[left][i] && actives[right][i]) {
                cost += weights[i];
            }
        }
        
        actives[n][i] = (actives[left][i] | actives[right][i]) & ISAPPLIC;
        
        // Reset the left buffers
        dnset[left][i]    = tempdn[left][i];
        dnsetf[left][i]   = tempdnf[left][i];
        prupset[left][i]  = tempprup[left][i];
//        upset[left][i]    = tempup[left][i];
        actives[left][i]  = tempact[left][i];
        // Reset the right buffers
        dnset[right][i]   = tempdn[right][i];
        dnsetf[right][i]  = tempdnf[right][i];
        prupset[right][i] = tempprup[right][i];
//        upset[right][i]   = tempup[right][i];
        actives[right][i] = tempact[right][i];

    }
    
    return cost;
}


int mpl_parsim_check_nas_updated(mpl_matrix* m)
{
    int i = 0;
    int ret = 0;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            ret += m->parsets[i].doeschange;
//            m->parsets[i].doeschange = 0;
        }
    }
    
    return ret;
}


/// Local reoptimsation functions for inapplicable characters

double mpl_fitch_na_local_check
(const double lim,
 const double base,
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
    const long end   = pd->end;
    double score     = 0.0;
//    long splits = 0;
    double cminscore = pd->cminscore; // Sum of all applicable changes
    double testscore = 0.0;
    double recall    = pd->crecall;
    
    for (i = pd->start; i < end; ++i) {
        if (upset[src][i] & ISAPPLIC) {
            if (rtset[tgt1][i] & ISAPPLIC) {
                if (!(rtset[tgt1][i] & upset[src][i])) {
                    score += weights[i];
                }
            } else if (upset[src][i] < UNKNOWN) {
//                if ((tempdn[tgt1][i] & ISAPPLIC) || (tempdn[tgt2][i] & ISAPPLIC)) {
                    pd->indexbuf[pd->nchars] = i;
                    ++pd->nchars;
                    pd->scorerecall += (changes[i] * weights[i]);
                    // using changes[] is POSSIBLY less accurate but much faster
                    pd->minscore    += (changes[i] * weights[i]);
//                    pd->minscore    += (applicchgs[i] * weights[i]);
//                } else {
//                    pd->indexbuf[pd->nchars] = i;
//                    ++pd->nchars;
//                    pd->scorerecall += (changes[i] * weights[i]);
//                    // using changes[] is less accurate but much faster
//                    // pd->minscore    += (changes[i] * weights[i]);
//                    pd->minscore    += (applicchgs[i] * weights[i]);
//                }
            }
        } else {
            if (rtset[tgt1][i] & NA) {
                if (tempact[troot][i] && tempact[src][i]) {
                    score += weights[i];
                }
            } else {
                pd->indexbuf[pd->nchars] = i;
                ++pd->nchars;
                pd->scorerecall += (changes[i] * weights[i]);
                pd->minscore    += (changes[i] * weights[i]);
            }
        }
        
//        assert(recall >= 0);
        // NOTE: It's possible that the complexity of checking this offsets the
        // efficiency of terminating the loop early.
        if (lim > -1.0) {
//            if ((score + base - pd->scorerecall + pd->minscore) > lim) {
//                pd->minscore += (score + base);
//                pd->scorerecall += recall;
//                return score + base;
//            }
            cminscore -= (applicchgs[i] * weights[i]);
            recall -= (changes[i] * weights[i]);
            testscore = score + pd->minscore + base + cminscore - pd->scorerecall - recall;
            if (testscore > lim) {
                pd->minscore += cminscore;
                pd->scorerecall += recall;
                return score;
            }
        }
    }
    
    return score;
}

/// Wagner parsimony functions

static inline unsigned int mpl_smallest_closed_interval
(mpl_discr* res, mpl_discr a, mpl_discr b)
{
    unsigned int steps = 0;
    mpl_discr c = 0;
    mpl_discr d = 0;
    
    if (b > a) {
        c = b;
        b = a;
        a = c;
    }
    
    d = a & (-a);
    
    while(!(d & b)) {
        ++steps;
        d |= a >> steps;
    }
    
    if (res != NULL) {
        *res = d;
    }
    
    return steps;
}

static inline unsigned int mpl_largest_closed_interval
 (mpl_discr* res, mpl_discr a, mpl_discr b)
{
    mpl_discr c = 0;
    mpl_discr d = 0;
    
    if (b > a) {
        c = b;
        b = a;
        a = c;
    }
    
    d = a;
    
    while(!(d & b)) {
        d |= d >> 1;
    }
    d |= b;
    
    if (res != NULL) {
        *res = d;
    }
    
    return 0;
}

double mpl_wagner_downpass
(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    double cost = 0.0;
    
    for (i = pd->start; i < end; ++i) {
        
        dnset[n][i] = dnset[left][i] & dnset[right][i];
        
        if (!dnset[n][i]) {
            cost += (weights[i] * mpl_smallest_closed_interval(&dnset[n][i],
                                                             dnset[left][i],
                                                             dnset[right][i]));
            ++changes[i];
        }
    }
    
    pd->score += cost;
    
    return cost;
}

void mpl_wagner_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    mpl_discr fin = 0UL;
    mpl_discr du;
    mpl_discr x;
    for (i = pd->start; i < end; ++i) {
        
        fin = upset[anc][i] & dnset[n][i];
        
        if (fin != upset[anc][i]) {
            du = dnset[left][i] | dnset[right][i];
            if (du & upset[anc][i]) {
                x = (du | dnset[n][i]) & upset[anc][i];
                if (x & dnset[n][i]) {
                    fin = x;
                } else {
                    if (x > dnset[n][i]) {
                        while (!(x & dnset[n][i])) {
                            x |= (x >> 1);
                        }
                    } else {
                        while (!(x & dnset[n][i])) {
                            x |= (x << 1);
                        }
                    }
                    fin = x;
                }
            } else {
                if (du > upset[anc][i]) {
                    du = du & (-du);
                } else {
                    du = mpl_discr_hibit(du);
                }
                
                if (dnset[n][i] > upset[anc][i]) {
                    x = dnset[n][i] & (-dnset[n][i]);
                } else {
                    x = mpl_discr_hibit(dnset[n][i]);
                }
                mpl_smallest_closed_interval(&fin, x, du);
            }
        }
        upset[n][i] = fin;

        if (upset[n][i] & upset[anc][i]) {
            rtset[n][i] = (upset[n][i] | upset[anc][i]);
        } else {
            mpl_smallest_closed_interval(&rtset[n][i], upset[n][i], upset[anc][i]);
            rtset[n][i] |= (upset[n][i] | upset[anc][i]);
        }
        
//        Ronquist version (doesn't work)
//        if (fin != upset[anc][i]) {
////            if ((dnset[left][i] | dnset[right][i]) & upset[anc][i]) {
//            mpl_parsim_closed_interval(&fin, dnset[left][i], dnset[right][i]);
//            fin |= (dnset[left][i] | dnset[right][i]); // Creates largest closed interval
//            fin = (fin & upset[anc][i]) | dnset[n][i];
//        }
//        upset[n][i] = fin;
    }
}

double mpl_wagner_local_check
(const double lim,
 const double base,
 const long src,
 const long tgt1,
 const long tgt2,
 const long troot,
 mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    double score = 0.0;
    
    for (i = pd->start; i < end; ++i) {
        if (!(rtset[tgt1][i] & dnset[src][i])) {
            score += (weights[i] * mpl_smallest_closed_interval(NULL,
                                                              rtset[tgt1][i],
                                                              dnset[src][i]));
        }
    }

    return score;
}

double mpl_wagner_na_second_downpass
(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i;
    long end = pd->end;
    mpl_discr t = 0;
    double cost = 0.0;
    long lchanges = 0;
    
//    pd->nndindices[n] = 0L;
    
    for (i = pd->start; i < end; ++i) {
        
        nodechanges[n][i] = 0L;
        
        if (prupset[n][i] & ISAPPLIC) {
            
            t = dnsetf[left][i] & dnsetf[right][i];
            
            if (t) {
                if (t & ISAPPLIC) {
                    dnsetf[n][i] = t & ISAPPLIC;
                } else {
                    dnsetf[n][i] = t;
                }
            } else {
                dnsetf[n][i] = (dnsetf[left][i] | dnsetf[right][i]) & ISAPPLIC;
                if (dnsetf[left][i] & ISAPPLIC && dnsetf[right][i] & ISAPPLIC) {
                    lchanges = mpl_smallest_closed_interval(&dnsetf[n][i],
                                                            dnsetf[left][i] & ISAPPLIC,
                                                            dnsetf[right][i] & ISAPPLIC);
                    cost += (weights[i] * lchanges);
                    changes[i] += lchanges;
                    applicchgs[i] += lchanges;
                    nodechanges[n][i] = lchanges;
                } else if (actives[left][i] && actives[right][i]) {
                    cost += weights[i];
                    ++changes[i];
                    nodechanges[n][i] = 1L;
                }
            }
        } else {
            dnsetf[n][i] = prupset[n][i];
            if (actives[left][i] && actives[right][i]) {
                cost += weights[i];
                ++changes[i];
                nodechanges[n][i] = 1L;
            }
        }
#ifdef DEBUG
        assert(dnsetf[n][i]);
#endif
        actives[n][i] = (actives[left][i] | actives[right][i]) & ISAPPLIC;
        
        tempact[n][i] = actives[n][i];
        tempdnf[n][i] = dnsetf[n][i];
    }
    
    pd->score += cost;
    
    return cost;
}

double mpl_wagner_na_down_reroot
 (const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i;
    long end = pd->end;
    mpl_discr t = 0;
    double cost = 0.0;
    long lchanges = 0;
    
    //    pd->nndindices[n] = 0L;
    
    for (i = pd->start; i < end; ++i) {
        
        nodechanges[n][i] = 0L;
        
        if (prupset[n][i] & ISAPPLIC) {
            
            t = dnsetf[left][i] & dnsetf[right][i];
            
            if (t) {
                if (t & ISAPPLIC) {
                    dnsetf[n][i] = t & ISAPPLIC;
                } else {
                    dnsetf[n][i] = t;
                }
            } else {
                dnsetf[n][i] = (dnsetf[left][i] | dnsetf[right][i]) & ISAPPLIC;
                if (dnsetf[left][i] & ISAPPLIC && dnsetf[right][i] & ISAPPLIC) {
                    lchanges = mpl_smallest_closed_interval(&dnsetf[n][i],
                                                           dnsetf[left][i] & ISAPPLIC,
                                                           dnsetf[right][i] & ISAPPLIC);
                    nodechanges[n][i] = lchanges;
                } else if (actives[left][i] && actives[right][i]) {
                    nodechanges[n][i] = 1L;
                }
            }
        } else {
            dnsetf[n][i] = prupset[n][i];
            if (actives[left][i] && actives[right][i]) {
                nodechanges[n][i] = 1L;
            }
        }
       
        actives[n][i] = (actives[left][i] | actives[right][i]) & ISAPPLIC;
        tempact[n][i] = actives[n][i];
        tempdnf[n][i] = dnsetf[n][i];
    }
    
    pd->score += cost;
    
    return cost;
}

void mpl_wagner_na_second_uppass
(const long left, const long right, const long n, const long anc, mpl_parsdat* pd)
{
    long i;
    const long end = pd->end;
    mpl_discr fin = 0UL;
    mpl_discr du;
    mpl_discr x;
    
    for (i = pd->start; i < end; ++i) {
        
        if (dnsetf[n][i] & ISAPPLIC) {
            if (upset[anc][i] & ISAPPLIC) {
                fin = upset[anc][i] & dnsetf[n][i];
                if (fin != upset[anc][i]) {
                    du = (dnsetf[left][i] | dnsetf[right][i]);// & ISAPPLIC;
                    if (du & NA) {
                        fin = NOCHARS;
                        if (!(dnsetf[n][i] & upset[anc][i])) {
                            mpl_smallest_closed_interval(&fin, dnsetf[n][i], upset[anc][i]);
                        }
                        fin |= (dnsetf[n][i] | upset[anc][i]);
                    } else {
                        if (du & upset[anc][i]) {
                            x = (du | dnsetf[n][i]) & upset[anc][i];
                            if (x & dnsetf[n][i]) {
                                fin = x;
                            } else {
                                if (x > dnsetf[n][i]) {
                                    while (!(x & dnsetf[n][i])) {
                                        x |= (x >> 1);
                                    }
                                } else {
                                    while (!(x & dnsetf[n][i])) {
                                        x |= (x << 1);
                                    }
                                }
                                fin = x;
                            }
                        } else {
                            if (du > upset[anc][i]) {
                                du = du & (-du);
                            } else {
                                du = mpl_discr_hibit(du);
                            }
                            
                            if (dnsetf[n][i] > upset[anc][i]) {
                                x = dnsetf[n][i] & (-dnsetf[n][i]);
                            } else {
                                x = mpl_discr_hibit(dnsetf[n][i]);
                            }
                            mpl_smallest_closed_interval(&fin, x, du);
                        }
                    }
                }
                upset[n][i] = fin;  // B1
            }
            else {
                upset[n][i] = dnsetf[n][i];
            }
        }
        else {
            upset[n][i] = dnsetf[n][i];
        }
        
        if (upset[n][i] & upset[anc][i]) {
            rtset[n][i] = (upset[n][i] | upset[anc][i]);
        } else {
            if (!((upset[n][i] | upset[anc][i]) & NA)) {
                mpl_smallest_closed_interval(&rtset[n][i], upset[n][i], upset[anc][i]);
                rtset[n][i] |= (upset[n][i] | upset[anc][i]);
            } else {
                rtset[n][i] = (upset[n][i] | upset[anc][i]);
            }
            
        }
        
        tempup[n][i] = upset[n][i];
//        assert(!(upset[n][i] & NA && upset[n][i] & ISAPPLIC));
//        assert(upset[n][i] & ISAPPLIC || upset[n][i] == NA);
#ifdef DEBUG
        assert(upset[n][i]);
#endif
    }
}

double mpl_wagner_na_local_check
(const double lim,
 const double base,
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
    const long end   = pd->end;
    double score     = 0.0;
    double cminscore = pd->cminscore; // Sum of all applicable changes
    double testscore = 0.0;
    double recall    = pd->crecall;
//    mpl_discr t = 0UL;
    
    for (i = pd->start; i < end; ++i) {
        if (upset[src][i] & ISAPPLIC) {
            if ((rtset[tgt1][i]) & ISAPPLIC) {
                if (!(rtset[tgt1][i] & upset[src][i])) {
                    score += (weights[i] * mpl_smallest_closed_interval(NULL,
                                                                      rtset[tgt1][i] & ISAPPLIC,
                                                                      upset[src][i]));
                }
            } else if (upset[src][i] < MISSING) {
                if ((tempdn[tgt1][i] & ISAPPLIC) || (tempdn[tgt2][i] & ISAPPLIC)) {
                    pd->indexbuf[pd->nchars] = i;
                    ++pd->nchars;
                    pd->scorerecall += (changes[i] * weights[i]);
                    pd->minscore    += (applicchgs[i] * weights[i]);
                } else {
                    pd->indexbuf[pd->nchars] = i;
                    ++pd->nchars;
                    pd->scorerecall += (changes[i] * weights[i]);
                    pd->minscore    += (applicchgs[i] * weights[i]);
                }
            }
        } else {
            if (rtset[tgt1][i] & NA) {
                if (tempact[troot][i] && tempact[src][i]) {
                    score += weights[i];
                }
            } else {
                pd->indexbuf[pd->nchars] = i;
                ++pd->nchars;
                pd->scorerecall += (changes[i] * weights[i]);
                pd->minscore    += (changes[i] * weights[i]);
            }
        }
        
//        assert(recall >= 0);
        // NOTE: It's possible that the complexity of checking this offsets the
        // efficiency of terminating the loop early.
        if (lim > -1.0) {
            cminscore -= (applicchgs[i] * weights[i]);
            recall -= (changes[i] * weights[i]);
            testscore = score + pd->minscore + cminscore + base - pd->scorerecall - recall;
            if (testscore > lim) {
                pd->minscore += cminscore;
                pd->scorerecall += recall;
                return score;
            }
        }
    }
    
    return score;
}

double mpl_wagner_na_recalc_second_downpass
 (const long left, const long right, const long n, mpl_parsdat* pd)
{
    size_t i;
    size_t j;
    long end = pd->end;
    mpl_discr t = 0;
    double cost = 0.0;
    
    long*  indices = pd->indexbuf;
    end = pd->nchars;
    
    // If possible, limit reconstructions to characters updated at this node.
    if (pd->usndidx == true) {
        indices = pd->ndindexbufs[n];
        end = pd->nndindices[n];
    }

    for (j = 0; j < end; ++j) {
        
        i = indices[j];
        
        // Deduct the old score added at this node for this character
        cost -= (nodechanges[n][i] * weights[i]);

        if (prupset[n][i] & ISAPPLIC) {
            
            t = dnsetf[left][i] & dnsetf[right][i];
            
            if (t) {
                if (t & ISAPPLIC) {
                    dnsetf[n][i] = t & ISAPPLIC;
                } else {
                    dnsetf[n][i] = t;
                }
            } else {
                dnsetf[n][i] = (dnsetf[left][i] | dnsetf[right][i]) & ISAPPLIC;
                if (dnsetf[left][i] & ISAPPLIC && dnsetf[right][i] & ISAPPLIC) {
                    cost += (weights[i] *
                             mpl_smallest_closed_interval(&dnsetf[n][i],
                                                        dnsetf[left][i] & ISAPPLIC,
                                                        dnsetf[right][i] & ISAPPLIC));
                } else if (actives[left][i] && actives[right][i]) {
                    cost += weights[i];
                }
            }
        } else {
            dnsetf[n][i] = prupset[n][i];
            if (actives[left][i] && actives[right][i]) {
                cost += weights[i];
            }
        }
        
        actives[n][i] = (actives[left][i] | actives[right][i]) & ISAPPLIC;
        
        // Reset all the descendant buffers
        dnset[left][i]    = tempdn[left][i];
        dnsetf[left][i]   = tempdnf[left][i];
        prupset[left][i]  = tempprup[left][i];
        actives[left][i]  = tempact[left][i];
        dnset[right][i]   = tempdn[right][i];
        dnsetf[right][i]  = tempdnf[right][i];
        prupset[right][i] = tempprup[right][i];
        actives[right][i] = tempact[right][i];
    }
    

    return cost;
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
    int j = 0;
    
    for (i = 0; i < m->nparsets; ++i) {
        m->parsets[i].score = 0.0;
        // Re-set the nodal index buffers
        if (m->parsets[i].isNAtype == true) {
            for (j = 0; j < m->parsets[i].nnodes; ++j) {
                m->parsets[i].nndindices[j] = 0L;
            }
        }
    }
    
    memset(changes,     0, m->cbufs[MPL_DISCR_T].char_max * sizeof(long));
    memset(applicchgs,  0, m->cbufs[MPL_DISCR_T].char_max * sizeof(long));
    memset(nusplits,    0, m->cbufs[MPL_DISCR_T].char_max * sizeof(long));
    
    // TODO: Get rid of the 2 * thing
    for (i = 0; i < (2 * m->cbufs[MPL_DISCR_T].row_max); ++i) {
        memset(nodechanges[i],  0,  m->cbufs[MPL_DISCR_T].char_max * sizeof(long));
        memset(rtset[i],        0,  m->cbufs[MPL_DISCR_T].char_max * sizeof(mpl_discr));
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
            mpl_na_recalc_tip_update(n, anc, &m->parsets[i]);
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
    long*  indices = pd->indexbuf;
//    mpl_discr* dnsetn, dnsetfn, prupsetn, activesn, tempdnn, tempdnfn, temprupn, tempactn;
//    mpl_discr* dnsetan, dnsetfan, prupsetan, activesan, tempdnan, tempdnfan, temprupan, tempactan;
//
//    dnsetn = dnset[n];
//    dnsetfn = dnsetf[n];
//    prupsetn = prupset[n];
//    activesn = actives[n];
//    tempdnn = tempdn[n];
//    tempdnfn = tempdnf[n];
//    temprupn = tempprup[n];
//    tempactn = tempact[n];
//    dnsetan = dnset[anc];
//    dnsetfan = dnsetf[anc];
//    prupsetan = prupset[anc];
//    activesan = actives[anc];
//    tempdnan = tempdn[anc];
//    tempdnfan = tempdnf[anc];
//    temprupan = tempprup[anc];
//    tempactan = tempact[anc];
    
    if (anc > -1) {
        for (j = pd->nchars; j-- ; ) {
            
            i = indices[j];
            
            dnset[n][i]     = tempdn[n][i];
            dnsetf[n][i]    = tempdnf[n][i];
            prupset[n][i]   = tempprup[n][i];
//            upset[n][i]     = tempup[n][i];
            actives[n][i]   = tempact[n][i];
            dnset[anc][i]   = tempdn[anc][i];
            dnsetf[anc][i]  = tempdnf[anc][i];
            prupset[anc][i] = tempprup[anc][i];
//            upset[anc][i]   = tempup[anc][i];
            actives[anc][i] = tempact[anc][i];
        }
    } else {
        for (j = pd->nchars; j-- ; ) {
            
            i = indices[j];
            
            dnset[n][i]     = tempdn[n][i];
            dnsetf[n][i]    = tempdnf[n][i];
            prupset[n][i]   = tempprup[n][i];
//            upset[n][i]     = tempup[n][i];
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


double mpl_wagner_src_root(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i = 0;
    long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {
        dnset[n][i] = 0UL;
        if (!(upset[left][i] & upset[right][i])) {
            mpl_smallest_closed_interval(&dnset[n][i], upset[left][i], upset[right][i]);
            dnset[n][i] |= (upset[left][i] | upset[right][i]);
        } else {
            dnset[n][i] = (upset[left][i] | upset[right][i]);
        }
    }
    
    return 0.0;
}

double mpl_na_do_src_root(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i = 0;
    long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {
        
        upset[n][i] = upset[left][i] | upset[right][i];

        if (upset[n][i] & ISAPPLIC) {
            upset[n][i] &= ISAPPLIC;
        }
////
        tempup[n][i]  = upset[n][i];
    }
    
    return 0.0;
}

double mpl_wagner_na_do_src_root(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i = 0;
    long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {
        
        if (upset[left][i] & ISAPPLIC && upset[right][i] & ISAPPLIC) {
            if (upset[left][i] & upset[right][i]) {
                upset[n][i] = upset[left][i] | upset[right][i];
            } else {
                mpl_smallest_closed_interval(&upset[n][i], upset[left][i], upset[right][i]);
                upset[n][i] |= (upset[left][i] | upset[right][i]);
            }
        } else {
            upset[n][i] = upset[left][i] | upset[right][i];
        }

        if (upset[n][i] & ISAPPLIC) {
            upset[n][i] &= ISAPPLIC;
        }
        ////
        tempup[n][i]  = upset[n][i];
    }
    
    return 0.0;
}

double mpl_na_do_src_tip(const long n, mpl_parsdat* pd)
{
    long i = 0;
    long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {
        
        upset[n][i] = prupset[n][i] = dnsetf[n][i] = dnset[n][i];

        
        tempdn[n][i]  = dnset[n][i];
        tempdnf[n][i] = dnsetf[n][i];
        tempprup[n][i] = prupset[n][i];
        tempup[n][i]  = upset[n][i];
        
        actives[n][i] = dnset[n][i] & actmask[i];
        tempact[n][i] = actives[n][i];
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

void mpl_parsim_do_src_tip(const long n, mpl_matrix* m)
{
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            mpl_na_do_src_tip(n, &m->parsets[i]);
        }
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

void mpl_update_root_prupset(const long left, const long right, const long n, mpl_parsdat* pd)
{
    long i = 0;
    long end = pd->end;
    
    for (i = pd->start; i < end; ++i) {
        prupset[n][i] = prupset[left][i] | prupset[right][i];
        if (prupset[n][i] & ISAPPLIC) {
            prupset[n][i] &= ISAPPLIC;
        }
        tempprup[n][i] = prupset[n][i];
    }
}

void mpl_parsim_update_root_prupset(const long left, const long right, const long n, mpl_matrix* m)
{
    int i;
        
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            mpl_update_root_prupset(left, right, n, &m->parsets[i]);
        }
    }
}

void mpl_parsim_update_active_sets(const long left, const long right, const long n, mpl_matrix* m)
{
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            // TODO: This fitch NA first downpass might need to be removed?
            mpl_fitch_na_first_downpass(left, right, n, &m->parsets[i]);
            m->parsets[i].rrupdate(left, right, n, &m->parsets[i]);
        }
    }
}

double mpl_parsim_local_check
(const double lim, const double base, const long src, const long tgt1, const long tgt2, const long troot, mpl_matrix* m)
{
    double score = 0.0;
    double cscore = base;
    int i;
    
    for (i = 0; i < m->nparsets; ++i) {
        m->parsets[i].scorerecall = 0.0;
        m->parsets[i].nchars = 0;
        m->parsets[i].minscore = 0.0;
        score += m->parsets[i].locfxn(lim, cscore, src, tgt1, tgt2, troot, &m->parsets[i]);
        cscore += score;
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

double mpl_parsim_calc_abs_minscore(mpl_matrix* m)
{
    size_t i = 0;
    size_t j = 0;
    double minscore = 0.0;

    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            m->parsets[i].cminscore = 0.0;
            m->parsets[i].crecall   = 0.0;
            for (j = m->parsets[i].start; j < m->parsets[i].end; ++j) {
                m->parsets[i].cminscore += (applicchgs[j] * weights[j]);
                m->parsets[i].crecall   += (changes[j] * weights[j]);
            }
            assert(m->parsets[i].crecall >= 0);
            minscore += m->parsets[i].cminscore;
        } else {
            for (j = m->parsets[i].start; j < m->parsets[i].end; ++j) {
                minscore += (changes[j] * weights[j]);
            }
        }
    }
    
    return minscore;
}

void mpl_reset_state_buffs(const long nrows, mpl_parsdat* pd)
{
    long i = 0;
    long j = 0;
    long k = 0;
    long*  indices = pd->indexbuf;
    
    if (pd->nchars > 0) {
//        if ((pd->indexbuf[pd->nchars-1] - pd->indexbuf[0] + 1) % 2) {
            for (i = 0; i < nrows; ++i) {
//                for (j = pd->indexbuf[0]; j <= pd->indexbuf[pd->nchars-1]; ++j) {
                    for (k = pd->nchars; k-- ; ) {
                    
                    j = indices[k];
                    dnset[i][j]   = tempdn[i][j];
                    prupset[i][j] = tempprup[i][j];
                    dnsetf[i][j]  = tempdnf[i][j];
                    upset[i][j]   = tempup[i][j];
                    actives[i][j] = tempact[i][j];
                }
            }
//        }
//        else {
//            for (i = 0; i < nrows; ++i) {
//                for (j = pd->indexbuf[0]; j <= pd->indexbuf[pd->nchars-1]; j += 2) {
//
//                    dnset[i][j]   = tempdn[i][j];
//                    prupset[i][j] = tempprup[i][j];
//                    dnsetf[i][j]  = tempdnf[i][j];
//                    upset[i][j]   = tempup[i][j];
//                    actives[i][j] = tempact[i][j];
//
//                    dnset[i][j+1]   = tempdn[i][j+1];
//                    prupset[i][j+1] = tempprup[i][j+1];
//                    dnsetf[i][j+1]  = tempdnf[i][j+1];
//                    upset[i][j+1]   = tempup[i][j+1];
//                    actives[i][j+1] = tempact[i][j+1];
//                }
//            }
//        }
    }
    
}



void mpl_parsim_reset_state_buffers(mpl_matrix *m)
{
    int i;

    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            mpl_reset_state_buffs(2 * m->num_rows, &m->parsets[i]);
        }
    }
}

void mpl_parsim_use_nodeidx(const bool useflag, mpl_matrix* m)
{
    long i = 0;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            m->parsets[i].usndidx = useflag;
        }
    }
}

void mpl_parsim_zero_na_nodal_changes(const long n, mpl_matrix *m)
{
    long i = 0;
    long j = 0;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            m->parsets[i].nndindices[n] = 0;
            for (j = m->parsets[i].start; j < m->parsets[i].end; ++j) {
                nodechanges[n][j] = 0L;
                prupset[n][j] = 0;
                tempprup[n][j] = 0;
            }
        }
    }
}

void mpl_parsim_reset_indexbufs(const long n, mpl_matrix *m)
{
    long i = 0;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            m->parsets[i].nndindices[n] = 0;
        }
    }
}

void mpl_parsim_reset_nodal_indexbufs(mpl_matrix *m)
{
    long i = 0;
    long j = 0;
    
    for (i = 0; i < m->nparsets; ++i) {
        if (m->parsets[i].isNAtype == true) {
            for (j = 0; j < m->parsets[i].nnodes; ++j) {
                m->parsets[i].nndindices[j] = 0;
            }
        }
    }
}

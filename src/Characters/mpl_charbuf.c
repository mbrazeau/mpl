//
//  mpl_charbuf.c
//  Morphy
//
//  Created by Martin Brazeau on 23/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "mpl_utils.h"
#include "mpl_charbuf.h"
#include "mpl_charinfo.h"
#include "mpl_matrix.h"

static void mpl_charbuf_init_datatype(mpl_charbuf* cb);
static void mpl_charbuf_delete_discr_buffer(long nrows, mpl_discr*** db);
static mpl_discr** mpl_charbuf_alloc_discr_buffer(long nrows, long ncols);
static void mpl_charbuf_setup_discrete_type(mpl_charbuf* cb);
static long** mpl_charbuf_alloc_long_matrix(long nrows, long ncols);

void mpl_charbuf_init
(const mpl_data_t datype, const long nrows, const long ncols, mpl_charbuf* cb)
{
    long i = 0;
    
    cb->num_rows = nrows; // Automatically size the rows as data will be added dynamically by column
    cb->num_chars = 0;
    cb->char_max = ncols;
    cb->row_max = nrows;
    cb->datype = datype;
    
    mpl_charbuf_init_datatype(cb);
    
    cb->weights         = (double*)safe_calloc(ncols, sizeof(double));
    cb->preweight       = (double*)safe_calloc(ncols, sizeof(double));
    cb->minchanges      = (long*)safe_calloc(ncols, sizeof(double));
    cb->orig_indices    = (long*)safe_calloc(ncols, sizeof(long));
    cb->charchanges     = (long*)safe_calloc(ncols, sizeof(long));
    cb->appliccanges    = (long*)safe_calloc(ncols, sizeof(long));
    cb->nusplits        = (long*)safe_calloc(ncols, sizeof(long));
    // TODO: CHECK RETURN!!!
    
    for (i = 0; i < cb->char_max; ++i) {
        cb->weights[i]   = DEFAULT_WEIGHT;
        cb->preweight[i] = DEFAULT_PREWEIGHT;
    }
}

void mpl_charbuf_cleanup(mpl_charbuf* cb)
{
    long i = 0;
    
    safe_free(cb->weights);
    safe_free(cb->preweight);
    safe_free(cb->orig_indices);
    safe_free(cb->minchanges);
    safe_free(cb->charchanges);
    safe_free(cb->appliccanges);
    safe_free(cb->nusplits);
    
    for (i = 0; i < (2 * cb->num_rows); ++i) {
        // Free the index bufs
        safe_free(cb->nodechanges[i]);
    }
    safe_free(cb->nodechanges);
    safe_free(cb->actmask);
    mpl_charbuf_delete_discr_buffer(2 * cb->num_rows, &cb->dnset);
    mpl_charbuf_delete_discr_buffer(2 * cb->num_rows, &cb->upset);
    mpl_charbuf_delete_discr_buffer(2 * cb->num_rows, &cb->prupset);
    mpl_charbuf_delete_discr_buffer(2 * cb->num_rows, &cb->dnsetf);
    mpl_charbuf_delete_discr_buffer(2 * cb->num_rows, &cb->rtset);
    mpl_charbuf_delete_discr_buffer(2 * cb->num_rows, &cb->actives);
    mpl_charbuf_delete_discr_buffer(2 * cb->num_rows, &cb->tempdn);
    mpl_charbuf_delete_discr_buffer(2 * cb->num_rows, &cb->tempprup);
    mpl_charbuf_delete_discr_buffer(2 * cb->num_rows, &cb->tempdnf);
    mpl_charbuf_delete_discr_buffer(2 * cb->num_rows, &cb->tempup);
    mpl_charbuf_delete_discr_buffer(2 * cb->num_rows, &cb->tempact);
    
    memset(cb, 0, sizeof(mpl_charbuf));
}

void mpl_charbuf_fill_std_column
(const long col, mpl_charinfo* ci, mpl_matrix* m, mpl_charbuf* cb)
{
    int i = 0;
    
    for (i = 0; i < m->num_rows; ++i) {
        // TODO: For this and related fxns: this will need a conditional for included taxa
        cb->dnset[i][cb->num_chars] = m->bitmatrix[i][col];
    }
    
//    assert(col == ci->index);
    ci->wtptr = &cb->weights[cb->num_chars];
    cb->orig_indices[cb->num_chars] = col;
    cb->weights[cb->num_chars] = ci->weight;
    
    ++cb->num_chars;
}

void mpl_charbuf_fill_std_additv_column
(const long col, mpl_charinfo* ci, mpl_matrix* m, mpl_charbuf* cb)
{
    int i = 0;
    int j = 0;
    int max = 0;
    mpl_discr d = 0;
    
    max = ci->maxstate;
    
    for (i = 0; i < m->num_rows; ++i) {
        
        d = m->bitmatrix[i][col];
        if (d == MISSING) {
            for (j = 0; j < max; ++j) {
                cb->dnset[i][cb->num_chars + j] = MISSING;
            }
            continue;
        }
        
        d = d >> 1;
        for (j = 0; j < max; ++j) {
            if (d == 0) {
                cb->dnset[i][cb->num_chars + j] = 0x01;
            } else {
                cb->dnset[i][cb->num_chars + j] = 0x02;
            }
            d = d >> 1;
        }
    }
    
//    assert(col == ci->index);
    
    ci->wtptr = &cb->weights[cb->num_chars];
    for (i = 0; i < max; ++i) {
        cb->orig_indices[cb->num_chars + i] = col;
        cb->weights[cb->num_chars + i] = ci->weight;
    }
    
    cb->num_chars += max;
}

void mpl_charbuf_fill_na_column
(const long col, mpl_charinfo* ci, mpl_matrix* m, mpl_charbuf* cb)
{
    int i = 0;
    
    cb->actmask[cb->num_chars] = ISAPPLIC;
    for (i = 0; i < m->num_rows; ++i) {
        // TODO: For this and related fxns: this will need a conditional for included taxa
        cb->dnset[i][cb->num_chars]   = m->bitmatrix[i][col];
        cb->tempdn[i][cb->num_chars]  = m->bitmatrix[i][col];
        cb->prupset[i][cb->num_chars] = m->bitmatrix[i][col];
        cb->upset[i][cb->num_chars]   = m->bitmatrix[i][col];
//        cb->actives[i][cb->num_chars] = m->bitmatrix[i][col] & ISAPPLIC;
//        cb->tempact[i][cb->num_chars] = m->bitmatrix[i][col] & ISAPPLIC;
    }
    
//    assert(col == ci->index);
    ci->wtptr = &cb->weights[cb->num_chars];
    cb->orig_indices[cb->num_chars] = col;
    cb->weights[cb->num_chars] = ci->weight;
    
    ++cb->num_chars;
}

void mpl_charbuf_fill_na_additv_column
(const long col, mpl_charinfo* ci, mpl_matrix* m, mpl_charbuf* cb)
{
    int i = 0;
    int j = 0;
    int max = 0;
    mpl_discr d = 0;
    
    max = ci->maxstate+1;
    
    for (i = 0; i < m->num_rows; ++i) {
        
        d = m->bitmatrix[i][col];
        if (d == MISSING) {
            
            cb->actmask[cb->num_chars] = ISAPPLIC;
            
            cb->dnset[i][cb->num_chars]   = MISSING;
            cb->tempdn[i][cb->num_chars]  = MISSING;
            cb->actives[i][cb->num_chars] = ISAPPLIC;
            cb->tempact[i][cb->num_chars] = ISAPPLIC;
            for (j = 1; j < max; ++j) {
                cb->dnset[i][cb->num_chars + j] = MISSING;
                cb->tempdn[i][cb->num_chars + j]  = MISSING;
                cb->actives[i][cb->num_chars + j] = 0;
                cb->tempact[i][cb->num_chars + j] = 0;
            }
            continue;
        }
        
        if (d == NA) {
            cb->actmask[cb->num_chars] = 0;
            
            cb->dnset[i][cb->num_chars]   = NA;
            cb->tempdn[i][cb->num_chars]  = NA;
            cb->actives[i][cb->num_chars] = 0;
            cb->tempact[i][cb->num_chars] = 0;
            // HERE: Set up the actives appropriately: 0
            
            for (j = 1; j < max; ++j) {
                cb->dnset[i][cb->num_chars + j]   = NA;
                cb->tempdn[i][cb->num_chars + j]  = NA;
                cb->actives[i][cb->num_chars + j] = 0;
                cb->tempact[i][cb->num_chars + j] = 0;
                // HERE: Set up the actives appropriately: 0
            }
            continue;
        }
        
//        d = d >> 1;
        
        cb->actmask[cb->num_chars] = ISAPPLIC;
        
        cb->dnset[i][cb->num_chars]     = 0x02;
        cb->tempdn[i][cb->num_chars]    = 0x02;
        cb->prupset[i][cb->num_chars]   = 0x02;
        cb->upset[i][cb->num_chars]     = 0x02;
        cb->actives[i][cb->num_chars]   = 0x02;
        cb->tempact[i][cb->num_chars]   = 0x02;
        
        d = d >> 1;
        
        for (j = 1; j < max; ++j) {
            cb->actmask[cb->num_chars + j] = 0;
            if (d == 0) {
                cb->dnset[i][cb->num_chars + j]   = 0x02;
                cb->tempdn[i][cb->num_chars + j]  = 0x02;
                cb->prupset[i][cb->num_chars + j] = 0x02;
                cb->upset[i][cb->num_chars + j]   = 0x02;
            } else {
                cb->dnset[i][cb->num_chars + j]   = 0x04;
                cb->tempdn[i][cb->num_chars + j]  = 0x04;
                cb->prupset[i][cb->num_chars + j] = 0x04;
                cb->upset[i][cb->num_chars + j]   = 0x04;
            }
            cb->actives[i][cb->num_chars + j] = 0;
            cb->tempact[i][cb->num_chars + j] = 0;
            d = d >> 1;
        }
    }
    
    ci->wtptr = &cb->weights[cb->num_chars];
    for (i = 0; i < max; ++i) {
        cb->orig_indices[cb->num_chars + i] = col;
        cb->weights[cb->num_chars + i] = ci->weight;
    }
    
    cb->num_chars += max;
}

void mpl_charbuf_add_data_column
(const mpl_discr* datcol, const long colnum, mpl_charinfo* ci, mpl_charbuf* cb)
{
    assert(cb->num_chars <= cb->char_max);
    
    long i = 0;
    
    // Set the weight directly at this point, but also set up the pointer
    // to the weight.
    ci->wtptr = &cb->weights[colnum];
    cb->weights[colnum] = ci->weight;
    
    cb->orig_indices[colnum] = ci->index;
    
    for (i = 0; i < cb->row_max; ++i) {
//        cb->upset[i][colnum] =
        cb->dnset[i][colnum]    = datcol[i];
        cb->tempdn[i][colnum]   = datcol[i];
        cb->upset[i][colnum]    = datcol[i];
        cb->prupset[i][colnum]  = datcol[i];
        cb->tempprup[i][colnum] = datcol[i];
        cb->tempup[i][colnum]   = datcol[i];
        cb->actives[i][colnum]  = datcol[i] & ISAPPLIC;
        cb->tempact[i][colnum]  = datcol[i] & ISAPPLIC;
//        cb->tempup[i][colnum] = datcol[i];
//        if (((datcol[i] - 1) & datcol[i]) == 0) {
//            cb->actives[i][colnum] = cb->tempact[i][colnum];
//        }
        assert(cb->dnset[i][colnum]);
    }
}


void mpl_charbuf_set_weight
(const long charnum, const double weight, mpl_charbuf* cb)
{
    cb->weights[charnum] = weight;
}


void mpl_charbuf_set_weights_equal(const double weight, mpl_charbuf* cb)
{
    long i = 0;
    for (i = 0; i < cb->char_max; ++i) {
        cb->weights[i] = weight;
    }
}


// TODO: These functions are not the  most efficient possible implememtation
// for this strategy Temp buffers could be written during full-pass optimisation
// and even then for only those characters that are being changed by the
// state set calculations.
void mpl_charbuf_store_discr_states(mpl_charbuf* cb)
{
    long i = 0;
    long j = 0;
    for (i = 2 * cb->row_max; i-- ;) {
        for (j = cb->char_max; j-- ;) {
            cb->tempdn[i][j]  =  cb->dnset[i][j] ;
            cb->tempup[i][j]  =  cb->upset[i][j] ;
            cb->tempact[i][j] =  cb->actives[i][j] ;
        }
//        memcpy(cb->tempdn[i],  cb->dnset[i],   cb->char_max * sizeof(mpl_discr));
//        memcpy(cb->tempup[i],  cb->upset[i],   cb->char_max * sizeof(mpl_discr));
//        memcpy(cb->tempact[i], cb->actives[i], cb->char_max * sizeof(mpl_discr));
    }
}

void mpl_charbuf_restore_discr_states
(const long start, const long end, mpl_charbuf* cb)
{
    size_t i;
//    size_t j;
    size_t lend = end;
    
    if (end == 0) {
        lend = cb->char_max;
    }
    
    for (i = 0; i < (2 * cb->row_max); ++i) {
        memcpy(&cb->dnset[i][start],
               &cb->tempdn[i][start],
               (lend - start) * sizeof(mpl_discr));
        memcpy(&cb->prupset[i][start],
               &cb->tempprup[i][start],
               (lend - start) * sizeof(mpl_discr));
        memcpy(&cb->upset[i][start],
               &cb->tempup[i][start],
               (lend - start) * sizeof(mpl_discr));
        memcpy(&cb->actives[i][start],
               &cb->tempact[i][start],
               (lend - start) * sizeof(mpl_discr));
//#pragma clang loop vectorize(enable)
//        for (j = start; j < lend; ++j) {
//            cb->dnset[i][j]     = cb->tempdn[i][j];
//            cb->prupset[i][j]   = cb->tempprup[i][j];
//            cb->upset[i][j]     = cb->tempup[i][j] ;
//            cb->actives[i][j]   = cb->tempact[i][j] ;
//        }
    }
}

//void mpl_charbuf_fast_restore_discr_states(const long nchar, const long* inds, mpl_charbuf* cb)
//{
//    long i = 0;
//    long rowmax = 2 * cb->row_max;
//    long j = 0;
//    long k = 0;
//
//    for (i = 0; i < rowmax; ++i) {
//        for (j = 0; j < nchar; ++j) {
////            k = inds[j];
//            cb->dnset[i][k]   =  cb->tempdn[i][k];
//            cb->upset[i][k]   =  cb->tempup[i][k];
//            cb->actives[i][k]  = cb->tempact[i][k];
//        }
//    }
//}

int mpl_charbuf_assert_temps_equal_bufs(mpl_charbuf* cb)
{
    int  ret = 0;
    long i = 0;
    long j = 0;
    long nrows = 2 * cb->row_max;
    
    for (i = 0; i < nrows; ++i) {
        for (j = 0; j < cb->char_max; ++j) {
            if (cb->dnset[i][j] != cb->tempdn[i][j]) {
                printf("buffer mismatch: row: %li, col: %li\n", i, j);
                printf("dnset: %u, tmpdn: %u\n", cb->dnset[i][j], cb->tempdn[i][j]);
            }
            if (cb->upset[i][j] != cb->tempup[i][j]) {
                printf("buffer mismatch: row: %li, col: %li\n", i, j);
                printf("upset: %u, tempup: %u\n", cb->upset[i][j], cb->tempup[i][j]);
            }
            if (cb->actives[i][j] != cb->tempact[i][j]) {
                printf("buffer mismatch: row: %li, col: %li\n", i, j);
                printf("actives: %u, tempact: %u\n", cb->actives[i][j], cb->tempact[i][j]);
            }
        }
    }
    
    return ret;
}


long mpl_charbuf_analyze_discr_minchanges(const long i, bool gapinapplic, mpl_charbuf* cb)
{
    mpl_discr d = 0;
    mpl_discr g = 0;
    long j = 0;
    long result = 0;
    
    if (gapinapplic == true) {
        g = NA;
    }
    
    for (j = 0; j < cb->num_rows; ++j) {
        if (cb->dnset[j][i] < UNKNOWN && cb->dnset[j][i] > g) {
            d |= cb->dnset[j][i];
        }
    }
    
    MPL_POPCOUNTLL(result, d);
    
    if (result > 0) {
        result -= 1;
    }
    
//    printf("Min number of changes for char %li:\t%li\n", i, result);
    
    return result;
}
/*******************************************************************************
 *                                                                             *
 *  PRIVATE FUNCTION DEFINITIONS                                               *
 *                                                                             *
 ******************************************************************************/

static void mpl_charbuf_init_datatype(mpl_charbuf* cb)
{
    switch (cb->datype) {
        case (MPL_DISCR_T):
            mpl_charbuf_setup_discrete_type(cb);
            break;
        case (MPL_CONTIN_T):
            // setup as continuous character block
            break;
        case (MPL_MODEL_T):
            // setup as a model-based character block
            break;
        default:
            break;
    }
}

static void mpl_charbuf_delete_discr_buffer(long nrows, mpl_discr*** db)
{
    mpl_discr** dbi = NULL;
    dbi = *db;
    
    long i = 0;
    
    for (i = 0; i < nrows; ++i) {
        safe_free(dbi[i]);
    }
    
    free(dbi);
    
    *db = NULL;
}

static mpl_discr** mpl_charbuf_alloc_discr_buffer(long nrows, long ncols)
{
    long i = 0;
    mpl_discr** newbuf = NULL;
    
    // TODO: This 2* thing needs to be replaced
    newbuf = (mpl_discr**)safe_calloc(2 * nrows, sizeof(mpl_discr*));
    
    if (newbuf == NULL) {
        return NULL;
    }
    
    for (i = 0; i < (2 * nrows); ++i) {
        newbuf[i] = (mpl_discr*)safe_calloc(ncols, sizeof(mpl_discr));
        if (newbuf[i] == NULL) {
            mpl_charbuf_delete_discr_buffer(nrows, &newbuf);
            return NULL;
        }
    }
    
    return newbuf;
}

static long** mpl_charbuf_alloc_long_matrix(long nrows, long ncols)
{
    long i = 0;
    long** ret = NULL;
    
    // TODO: This 2* thing needs to be replaced
    ret = (long**)safe_calloc(2 * nrows, sizeof(long*));
    
    for (i = 0; i < (2 * nrows); ++i) {
        ret[i] = (long*)safe_calloc(ncols, sizeof(long));
    }
    
    return ret;
}

static void mpl_charbuf_setup_discrete_type(mpl_charbuf* cb)
{
    assert(cb->row_max && cb->char_max);
    
    cb->actmask  = (mpl_discr*)safe_calloc(cb->char_max, sizeof(mpl_discr));
    cb->dnset    = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->prupset  = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->dnsetf   = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->upset    = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->actives  = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->rtset    = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->tempdn   = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->tempprup = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->tempdnf  = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->tempup   = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->tempact  = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    
    cb->nodechanges = mpl_charbuf_alloc_long_matrix(cb->row_max, cb->char_max);
    cb->regdist = (short**)alloc_matrix(2 * cb->row_max, cb->char_max, sizeof(short));
}

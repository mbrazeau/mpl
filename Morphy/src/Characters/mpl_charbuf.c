//
//  mpl_charbuf.c
//  Morphy
//
//  Created by Martin Brazeau on 23/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//
#include <assert.h>
#include <string.h>

#include "../mpl_utils.h"
#include "mpl_charbuf.h"
#include "mpl_charinfo.h"

static void mpl_charbuf_init_datatype(mpl_charbuf* cb);
static void mpl_charbuf_delete_discr_buffer(long nrows, mpl_discr*** db);
static mpl_discr** mpl_charbuf_alloc_discr_buffer(long nrows, long ncols);
static void mpl_charbuf_setup_discrete_type(mpl_charbuf* cb);


void mpl_charbuf_init
(const mpl_data_t datype, const long nrows, const long ncols, mpl_charbuf* cb)
{
    long i = 0;
    
    cb->num_rows = nrows; // Automatically size the rows as data will be added dynamically by column
    cb->num_chars = ncols;
    cb->char_max = ncols;
    cb->row_max = nrows;
    cb->datype = datype;
    
    mpl_charbuf_init_datatype(cb);
    
    cb->weights = (double*)safe_calloc(ncols, sizeof(double));
    cb->orig_indices = (long*)safe_calloc(ncols, sizeof(long));
    // TODO: CHECK RETURN!!!
    
    for (i = 0; i < cb->char_max; ++i) {
        cb->weights[i] = DEFAULT_WEIGHT;
    }
}


void mpl_charbuf_clear(mpl_charbuf* cb)
{
    if (cb->dnset != NULL) {
        free(cb->dnset);
        cb->dnset = NULL;
    }
    
    if (cb->upset != NULL) {
        free(cb->upset);
        cb->upset = NULL;
    }
    
    if (cb->actives != NULL) {
        free(cb->actives);
        cb->actives = NULL;
    }
    
    if (cb->tempdn != NULL) {
        free(cb->tempdn);
        cb->tempdn = NULL;
    }
    
    if (cb->tempup != NULL) {
        free(cb->tempup);
        cb->tempup = NULL;
    }
    
    if (cb->tempact != NULL) {
        free(cb->tempact);
        cb->tempact = NULL;
    }
    
    if (cb->weights != NULL) {
        free(cb->weights);
        cb->weights = NULL;
    }
    safe_free(cb->orig_indices);
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
        cb->dnset[i][colnum] = datcol[i];
    }
}

void mpl_charbuf_set_weight
(const long charnum, const double weight, mpl_charbuf* cb)
{
    cb->weights[charnum] = weight;
}

// TODO: These functions are not the  most efficient possible implememtation
// for this strategy Temp buffers could be written during full-pass optimisation
// and even then for only those characters that are being changed by the
// state set calculations.
void mpl_charbuf_store_discr_states(mpl_charbuf* cb)
{
    long i = 0;
    for (i = 0; i < cb->num_rows; ++i) {
        memcpy(cb->tempdn[i], cb->dnset[i], cb->char_max * sizeof(mpl_discr));
        memcpy(cb->tempup[i], cb->upset[i], cb->char_max * sizeof(mpl_discr));
        memcpy(cb->tempact[i], cb->actives[i], cb->char_max * sizeof(mpl_discr));
    }
}

void mpl_charbuf_restore_discr_states(mpl_charbuf* cb)
{
    long i = 0;
    for (i = 0; i < cb->num_rows; ++i) {
        memcpy(cb->dnset[i], cb->tempdn[i], cb->char_max * sizeof(mpl_discr));
        memcpy(cb->upset[i], cb->tempup[i], cb->char_max * sizeof(mpl_discr));
        memcpy(cb->actives[i], cb->tempact[i], cb->char_max * sizeof(mpl_discr));
    }
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
    mpl_discr** dbi = *db;
    
    long i = 0;
    
    for (i = 0; i < nrows; ++i) {
        if (dbi[i] != NULL) {
            free(dbi[i]);
            dbi[i] = NULL;
        }
    }
    
    free(dbi);
    
    *db = NULL;
}

static mpl_discr** mpl_charbuf_alloc_discr_buffer(long nrows, long ncols)
{
    long i = 0;
    mpl_discr** newbuf = NULL;
    
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

static void mpl_charbuf_setup_discrete_type(mpl_charbuf* cb)
{
    assert(cb->row_max && cb->char_max);
    
    cb->dnset   = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->upset   = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->actives = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->tempdn  = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->tempup  = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
    cb->tempact = mpl_charbuf_alloc_discr_buffer(cb->row_max, cb->char_max);
}

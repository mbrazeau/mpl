//
//  mpl_handle.c
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

//#include "mpl_handle.h"
#include <stdlib.h>

#include "mpl_defs.h"
#include "mpl_utils.h"
#include "mpl_taxblock.h"
#include "mpl_search.h"
#include "mpl_results.h"
#include "mpl_matrix.h"
#include "mpl_scoretree.h"
#include "mpl_newick_rdr.h"
#include "mpl_bbreak.h"

typedef struct _handle {
    
    long            ntax;
    long            nchar;
    mpl_taxablock*  taxablock;
    mpl_search*     search;
    mpl_results*    results;
    mpl_matrix*     matrix;
    mpl_treelist*   treebuf;
    mpl_newick_rdr* newickrdr;
    
} mpl_handle;

// Static function prototypes
static int mpl_check_init(mpl_handle* handl);

mpl_handle* mpl_handle_new(void)
{
    mpl_handle* newhandl = NULL;
    
    newhandl = (mpl_handle*)safe_calloc(1, sizeof(mpl_handle));
    
    if (newhandl != NULL) {
        newhandl->ntax      = 0;
        newhandl->nchar     = 0;
        newhandl->taxablock = NULL;
        newhandl->search    = NULL;
        newhandl->results   = NULL;
        newhandl->matrix    = NULL;
        newhandl->treebuf   = NULL;
        newhandl->newickrdr = NULL;
    }
    
    return newhandl;
}

int mpl_handle_delete(mpl_handle** handl)
{
    RET_IF_NULL(handl);
    
    //int ret = 0;
    
    // ret = Cleanup taxablock
    mpl_taxablock_delete(&(*handl)->taxablock);
    // ret = Cleanup search
    mpl_search_delete(&(*handl)->search);
    // ret = Cleanup results
    // ret = Cleanup matrix
    mpl_matrix_delete(&(*handl)->matrix);
    // ret = Cleanup treebuf
    mpl_treelist_delete(&(*handl)->treebuf);
    // ret = Cleanup newickrdr
    mpl_newick_rdr_delete(&(*handl)->newickrdr);

    free(*handl);
    *handl = NULL;

    return MPL_SUCCESS;
}


int mpl_reset_handle(mpl_handle* handl)
{
    //
    return MPL_NOTIMPLEMENTED;
}

int mpl_set_dimensions(const long ntax, const long nchar, mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    if (ntax == 0 || nchar == 0) {
        return MPL_NODIMENSIONS;
    }
    
    if (!mpl_check_init(handl)) {
        
        handl->ntax  = ntax;
        handl->nchar = nchar;
        
        handl->matrix  = mpl_matrix_new();
        if (handl->matrix == NULL) {
            return MPL_NOMEM;
        }
        
        mpl_matrix_init(ntax, nchar, 2 * ntax, handl->matrix);
        
        // Add taxablock
        handl->taxablock = mpl_taxablock_new(handl->ntax);
        
        // Add results
        // handl->results
        
        handl->treebuf = mpl_treelist_new(handl->ntax, MPL_DEFAULT_MAXTREES, MPL_DEFAULT_AUTOINCR);
        
        // Add search
        handl->search    = mpl_search_new(handl->ntax, handl->treebuf);
        // TODO: Probably not needed
        mpl_search_set_stepw_t(DEFAULT_ASTYPE, handl->search);
        mpl_search_set_bbreak_t(DEFAULT_BBREAK, handl->search);
        
        // TODO: Check all returns. If any are NULL, then cleanup and return error
        handl->newickrdr = mpl_newick_rdr_new(ntax);
        
        return MPL_SUCCESS;
    }
    
    return MPL_ILLEGOVERWRITE;
}

long mpl_get_ntax(const mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    return handl->ntax;
}

long mpl_get_nchar(const mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    return handl->nchar;
}

int mpl_attach_rawdata(const mpl_data_t datype, const char* rawmatrix, mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    MPL_RETURN ret = MPL_ERR;
    
    if (handl->nchar == 0 || handl->ntax == 0) {
        return MPL_NODIMENSIONS;
    }
    
    // TODO: Attach the data
    ret = mpl_matrix_attach_rawdata(rawmatrix, handl->matrix);
    
    return ret;
}

int mpl_attach_symbols(const char* symbols, mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    MPL_RETURN ret = MPL_ERR;
    
    // TODO: Attach the symbol set
    
    return ret;
}

int mpl_set_parsim_t(const long index, const mpl_parsim_t ptype, mpl_handle* handl)
{
    RET_IF_NULL(handl);
   
    // TODO: Need to check if this changes any existing settings and then re-set the types
    
    return mpl_matrix_set_parsim_t(index, ptype, handl->matrix);
}

int mpl_set_gap_handl(const mpl_gap_t gaphandl, mpl_handle* handl)
{
    mpl_matrix_set_gap_handle(gaphandl, handl->matrix);
    return MPL_SUCCESS;
}

int mpl_set_hold(const int hold, mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    mpl_search_set_hold(hold, handl->search);
    
    return MPL_SUCCESS;
}

int mpl_set_maxtrees(const long maxtrees, mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    mpl_treelist_set_maxtrees(maxtrees, handl->treebuf);
    
    return MPL_SUCCESS;
}

int mpl_set_addseq(const mpl_addseq_t as, mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    mpl_search_set_stepw_t(as, handl->search);
    
    return MPL_SUCCESS;
}

int mpl_set_brswap(const mpl_bbreak_t bbkt, mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    if (bbkt > MPL_BBREAK_MAX) {
        return MPL_BADPARAM;
    }
    
    mpl_bbreak_set_type(bbkt, handl->search->bbreak);
    // TODO: Don't keep doing it like this:
    handl->search->bbreak_type = bbkt;
    
    return MPL_SUCCESS;
}

int mpl_set_numreps(const unsigned long nreps, mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    mpl_search_set_numreps(nreps, handl->search);
    
    return MPL_SUCCESS;
}

long mpl_get_num_trees(mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    return mpl_treelist_get_numtrees(handl->treebuf);
}

int mpl_use_ratchet(const bool useratchet, mpl_handle* handl)
{
    RET_IF_NULL(handl)
    mpl_search_use_ratchet(useratchet, handl->search);
    return MPL_SUCCESS;
}

int mpl_do_search(mpl_handle* handl)
{
    RET_IF_NULL(handl);
    MPL_RETURN ret = MPL_ERR;
    
    ret = mpl_matrix_apply_data(handl->matrix);
    if (ret != MPL_SUCCESS) {
        return ret;
    }
    
    ret = mpl_init_parsimony(handl->matrix);
    if (ret != MPL_SUCCESS) {
        return ret;
    }
    
    ret = mpl_matrix_report(handl->matrix);
    if (ret != MPL_SUCCESS) {
        return ret;
    }
    
    ret = mpl_search_execute(handl->search);

    return ret;
}

int mpl_score_tree(double* score, const long index, mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    mpl_tree* t = NULL;
    mpl_topol* top = NULL;
    MPL_RETURN ret = MPL_ERR;
    
    if (index < 0 || index > handl->treebuf->num_trees) {
        return MPL_OUTOFBOUNDS;
    }
    
    // Needs to check if data is loaded and ready for analysis
    ret = mpl_matrix_apply_data(handl->matrix);
    if (ret != MPL_SUCCESS) {
        return ret;
    }
    
    ret = mpl_init_parsimony(handl->matrix);
    if (ret != MPL_SUCCESS) {
        return ret;
    }

    t = mpl_new_tree(handl->ntax); // TODO: This needs to get the active number of taxa in the dataset
    top = mpl_treelist_get_topol(index, handl->treebuf);
    
    if (top == NULL) {
        if (handl->treebuf->num_trees == 0) {
            return MPL_NOTREES;
        } else if (index >= handl->treebuf->num_trees) {
            return MPL_OUTOFBOUNDS;
        } else {
            return MPL_ERR;
        }
    }
    
    mpl_tree_read_topol(t, top);
    
    *score = mpl_fullpass_parsimony(t);
    
    mpl_delete_tree(&t);

    return ret;
}

int mpl_clear_trees(mpl_handle* handl)
{
    RET_IF_NULL(handl);
    MPL_RETURN ret = MPL_ERR;

    if (handl->treebuf != NULL) {
        mpl_treelist_clear_all(handl->treebuf);
        ret = MPL_SUCCESS;
    } else {
        ret = MPL_NOTREES;
    }

    return ret;
}

int mpl_add_newick(const char* newick, mpl_handle* handl)
{
    RET_IF_NULL(handl);
    MPL_RETURN ret = MPL_ERR;
    
    mpl_tree* t = mpl_new_tree(handl->ntax);
    
    mpl_topol* top = mpl_topol_new(handl->ntax); // Sub in fetch from list (can avoid 'regrowing' tree)
    mpl_newick_read(newick, top, handl->newickrdr);
    mpl_tree_read_topol(t, top);
    
    mpl_treelist_add_tree(false, t, handl->treebuf);
    
    mpl_topol_delete(&top);
    mpl_delete_tree(&t);
    
    return ret;
}

int mpl_get_num_states(const int i, mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    MPL_RETURN ret = MPL_ERR;
    
    if (i >= handl->nchar) {
        return MPL_OUTOFBOUNDS;
    }
    
    return mpl_matrix_get_num_states(i, handl->matrix);
}

char* mpl_get_newick(const long tnum, mpl_handle* handl)
{
    if (handl == NULL) {
        return NULL;
    }
    if (tnum >= mpl_treelist_get_numtrees(handl->treebuf)) {
        return NULL;
    }
    
    char* ret = NULL;
    
    mpl_tree* t  = mpl_new_tree(handl->treebuf->num_taxa);
    mpl_topol* top = mpl_treelist_get_topol(tnum, handl->treebuf);
    
    mpl_tree_read_topol(t, top);
    
    mpl_tree_write_newick(&ret, t);
    // TODO: Make this safer
    top->newick = ret;
    
    mpl_delete_tree(&t);
    
    return ret;
}

/*******************************************************************************
 *                                                                             *
 *  PRIVATE FUNCTION DEFINITIONS                                               *
 *                                                                             *
 ******************************************************************************/

/**
 Checks if the handle struct has been initialised. This is used when fundamental
 parameters like dimensions are being set for the first time.
 
 @param handl A pointer to an instance of the mpl_handle struct.
 @return An int indicating initialisation 1 and non-initialisation 0.
 */
static int mpl_check_init(mpl_handle* handl)
{
    if (handl->taxablock != NULL) {
        return 1;
    }
    
    if (handl->search != NULL) {
        return 1;
    }
    
    if (handl->results != NULL) {
        return 1;
    }
    
    if (handl->matrix != NULL) {
        return 1;
    }
    
    if (handl->treebuf != NULL) {
        return 1;
    }
    
    return 0;
}

static int mpl_handle_cleanup(mpl_handle handl)
{
    return MPL_ERR;
}

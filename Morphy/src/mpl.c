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
#include "Taxa/mpl_taxblock.h"
#include "Analysis/mpl_search.h"
#include "Results/mpl_results.h"
#include "Characters/mpl_matrix.h"
//typedef struct _taxablock   mpl_taxablock;
//typedef struct _search      mpl_search;
//typedef struct _results     mpl_results;
//typedef struct _matrix      mpl_matrix;

#define RET_IF_NULL(p) if (p == NULL) {return MPL_UNEXPNULLPTR;}


typedef struct _handle {
    
    long            ntax;
    long            nchar;
    mpl_taxablock*  taxablock;
    mpl_search*     search;
    mpl_results*    results;
    mpl_matrix*     matrix;
    mpl_treelist*   treebuf;
    
} mpl_handle;

// Static function prototypes
static int mpl_check_init(mpl_handle* handl);


/*******************************************************************************
 *                                                                             *
 *  PUBLIC FUNCTION DEFINITIONS                                                *
 *                                                                             *
 ******************************************************************************/

/**
 Creates a new instance of the mpl_handle. A program can generate as many of
 these as it needs.
 
 @return A pointer to an instance of an mpl_handle.
 */
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
    }
    
    return newhandl;
}

/**
 Deletes an instance of the mpl_handle struct by freeing all of its memory and
 setting the pointer to NULL.
 
 @param handl Address of the pointer to the handle structure.
 @return An mpl error code.
 */
int mpl_handle_delete(mpl_handle** handl)
{
    RET_IF_NULL(handl);
    
    int ret = 0;
    
    // ret = Cleanup taxablock
    // ret = Cleanup search
    // ret = Cleanup results
    // ret = Cleanup matrix
    // ret = Cleanup treebuf
    
    return ret;
}

/**
 Resets the parameters in the handle to initial values. All objects and
 allocated memory in the handle is freed/destroyed. This function is to be used
 when changing the dimensions and input data.
 
 @param handl A pointer to an instance of the mpl_handle struct.
 @return An mpl error code.
 */
int mpl_reset_handle(mpl_handle* handl)
{
    //
    return MPL_NOTIMPLEMENTED;
}

/**
 Initialise the dimensions of the data set to be supplied to the mpl. This will
 set the number of terminals (taxa) and characters. These values will be used
 to check data inputs.
 
 @param ntax The number of taxa (rows) in the dataset.
 @param nchar The number of characters (columns) in the dataset.
 @param handl A pointer to an instance of the mpl_handle struct.
 @return An mpl error code.
 */
int mpl_set_dimensions(const long ntax, const long nchar, mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    if (!mpl_check_init(handl)) {
        
        handl->ntax  = ntax;
        handl->nchar = nchar;
        
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

int mpl_attach_rawdata(const char* rawmatrix, mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    MPL_RETURN ret = MPL_SUCCESS;
    
    if (handl->nchar == 0 || handl->ntax == 0) {
        return MPL_NODIMENSIONS;
    }
    
    // TODO: Attach the data
    
    return ret;
}

int mpl_attach_symbols(const char* symbols, mpl_handle* handl)
{
    RET_IF_NULL(handl);
    
    MPL_RETURN ret = MPL_SUCCESS;
    
    // TODO: Attach the symbol set
    
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

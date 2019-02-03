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

typedef struct _handle {
    
    mpl_taxablock  taxablock;
    mpl_search     search;
    mpl_results    results;
    mpl_matrix     matrix;
    
} mpl_handle;

mpl_handle* mpl_handle_new(void)
{
    mpl_handle* newhandl = NULL;
    
    newhandl = (mpl_handle*)safe_calloc(1, sizeof(mpl_handle));
    
    return newhandl;
}



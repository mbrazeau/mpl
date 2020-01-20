//
//  mpl_taxblock.c
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include <assert.h>

#include "mpl_taxblock.h"
#include "mpl_utils.h"

mpl_taxablock* mpl_taxablock_new(const long max_taxa)
{
    if (!max_taxa) {
        return NULL;
    }
    
    mpl_taxablock* newtb = NULL;

    newtb = (mpl_taxablock*)safe_calloc(1, sizeof(mpl_taxablock));
    
    if (!newtb) {
        return NULL;
    }
    
    newtb->num_taxa = 0;
    newtb->max_taxa = max_taxa;
    
    newtb->taxa = (mpl_taxoninfo*)safe_calloc(max_taxa, sizeof(mpl_taxoninfo));
    
    if (!newtb->taxa) {
        if (newtb) {
            free(newtb);
            newtb = NULL;
        }
    }
    
    return newtb;
}

int mpl_taxablock_delete(mpl_taxablock** tb)
{
    if (!tb) {
        return -1;
    }
    
    if (!*tb) {
        return -1;
    }
    
    if ((*tb)->taxa) {
        free((*tb)->taxa);
        (*tb)->taxa = NULL;
    }
    
    free(*tb);
    *tb = NULL;
    
    return 0;
}

int mpl_taxablock_add_taxon(const char* label, mpl_taxablock* tb)
{
    if (!tb) {
        // TODO: These functions are so high level they should have error returns
        return -1;
    }
    
    if (tb->num_taxa == tb->max_taxa) {
        return -2;
    }
    
    mpl_taxoninfo_set_name(&tb->taxa[tb->num_taxa], label);
    mpl_taxoninfo_set_index(&tb->taxa[tb->num_taxa], tb->num_taxa);
    ++tb->num_taxa;
    
     assert(tb->num_taxa <= tb->max_taxa);
    
    return 0;
}

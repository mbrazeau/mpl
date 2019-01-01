//
//  mpl_taxblock.c
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include "mpl_taxblock.h"
#include "../mpl_utils.h"

mpl_taxablock* mpl_taxablock_new(const long num_taxa)
{
    if (!num_taxa) {
        return NULL;
    }
    
    mpl_taxablock* newtb = NULL;
    
    newtb->num_taxa = num_taxa;
    newtb->taxa = (mpl_taxoninfo*)safe_calloc(num_taxa, sizeof(mpl_taxoninfo));
    
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

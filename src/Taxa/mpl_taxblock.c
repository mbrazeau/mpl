//
//  mpl_taxblock.c
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include "mpl_taxblock.h"
#include "../mpl_utils.h"

mpl_taxablock* mpl_taxablock_new(long num_taxa)
{
    if (!num_taxa) {
        return NULL;
    }
    
    mpl_taxablock* newtb = NULL;
    
    newtb->num_taxa = num_taxa;
    newtb->taxa = (mpl_taxoninfo*)safe_calloc(num_taxa, sizeof(mpl_taxoninfo));
    
    if (!newtb->taxa) {
        mplfree(newtb);
    }
    
    return newtb;
}

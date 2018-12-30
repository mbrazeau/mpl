//
//  mpl_taxblock.h
//  Morphy
//
//  Created by Martin Brazeau on 30/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef mpl_taxblock_h
#define mpl_taxblock_h

#include "mpl_taxoninfo.h"

typedef struct _taxblock {
    long            num_taxa;
    mpl_taxoninfo*  taxa;
} mpl_taxablock;

mpl_taxablock*  mpl_taxablock_new(long num_taxa);
int             mpl_taxablock_delete(mpl_taxablock** tb);

#endif /* mpl_taxblock_h */

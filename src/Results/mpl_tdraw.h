//
//  mpl_tdraw.h
//  Morphy
//
//  Created by Martin Brazeau on 20/04/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_tdraw_h
#define mpl_tdraw_h

#include "mpl_tree.h"
#include "mpl_taxblock.h"

typedef struct _tdraw {
    
    int     width;
    int     height;
    char**  canvas;
    
} mpl_tdraw;

mpl_tdraw* mpl_trdraw_new(mpl_tree* t, mpl_taxablock* taxa);
void mpl_tdraw_delete(mpl_tdraw** td);
void mpl_tdraw_print(mpl_tdraw* td);

#endif /* mpl_tdraw_h */

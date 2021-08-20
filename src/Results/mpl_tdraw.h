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

#define DEFAULT_CANVAS_WIDTH 100
#define DEFAULT_TIP_COLUMN 83
#define NAME_BUFFER_WIDTH 15

typedef struct _tdraw {
    
    long                    width;  // Display width
    long                    height; // Display height
    long                    ttop;   // Top of the tree branches
    char**                  canvas;
    mpl_taxablock*          taxa;
    
} mpl_tdraw;

mpl_tdraw* mpl_tdraw_new(const mpl_taxablock *taxa);
void mpl_tdraw_delete(mpl_tdraw **td);
void mpl_tdraw_do(mpl_tree *t, mpl_tdraw *td);
void mpl_tdraw_print(mpl_tdraw *td);

#endif /* mpl_tdraw_h */

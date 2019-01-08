//
//  mpl_bbreak.h
//  Morphy
//
//  Created by Martin Brazeau on 07/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_bbreak_h
#define mpl_bbreak_h

#include "mpl_search.h"
#include "../Trees/mpl_node.h"
#include "../Trees/mpl_tree.h"

typedef struct _bbreak {
    
    mpl_bbreak_t    bbktype;
    long            num_nodes;
    mpl_node**      srcs;
    mpl_node**      tgts;
    unsigned long   num_rearrangs;

} mpl_bbreak;

mpl_bbreak* mpl_bbreak_new(long num_nodes);
void        mpl_bbreak_delete(mpl_bbreak** bbk);
void        mpl_do_bbreak(mpl_tree* t, mpl_bbreak* bbk);

#endif /* mpl_bbreak_h */

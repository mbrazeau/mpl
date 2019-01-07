//
//  mpl_bbreak.h
//  Morphy
//
//  Created by Martin Brazeau on 07/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_bbreak_h
#define mpl_bbreak_h

#include "../Trees/mpl_node.h"
#include "../Trees/mpl_tree.h"

typedef struct _bbreak {
    
    mpl_node**      srcs;
    mpl_node**      tgts;
    unsigned long   num_rearrangs;

} mpl_bbreak;

#endif /* mpl_bbreak_h */

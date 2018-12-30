//
//  mpl_cblock.h
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#ifndef mpl_cblock_h
#define mpl_cblock_h

typedef enum {
    MPL_DISCR_T,
    MPL_CONTIN_T,
    MPL_MODEL_T
} mpl_data_t;

typedef struct {
    
    long        num_taxa;
    long        num_nodes;
    long        num_chars;
    long**      data;
    mpl_data_t  type;
    
} mpl_cblock;

#endif /* mpl_cblock_h */

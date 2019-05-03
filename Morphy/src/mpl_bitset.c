//
//  mpl_bitset.c
//  Morphy
//
//  Created by Martin Brazeau on 31/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//
#include <assert.h>
#include "mpl_utils.h"
#include "mpl_bitset.h"

typedef struct _bitset {
    
    int nfields;
    int maxbit;
    unsigned long* data;
    
} mpl_bitset;

mpl_bitset* mpl_bitset_new(const int minbits)
{
    mpl_bitset* nb = NULL;
    nb = (mpl_bitset*)safe_calloc(1, sizeof(mpl_bitset));
    
    if (nb == NULL) {
        return NULL;
    }
    
    nb->nfields = minbits / NULONGBITS;
    
    if (minbits % NULONGBITS != 0) {
        nb->nfields += 1;
    }
    
    nb->data = NULL;
    nb->data = (unsigned long*)safe_calloc(nb->nfields, sizeof(unsigned long));
    
    if (nb->data == NULL) {
        safe_free(nb);
        nb = NULL; // Just precautionary
    }
    
    nb->maxbit = nb->nfields * NULONGBITS;
    
    return nb;
}

void mpl_bitset_delete(mpl_bitset** b)
{
    safe_free((*b)->data);
    (*b)->nfields = 0;
    (*b)->maxbit  = 0;
    safe_free(*b);
}

inline bool mpl_bitset_set(const long i, mpl_bitset* b)
{
    b->data[i / NULONGBITS] |= ( ((unsigned long)1) << (i % NULONGBITS) );
    
    return true;
}

//
//  mpl_bitset.c
//  Morphy
//
//  Created by Martin Brazeau on 31/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "mpl_utils.h"
#include "mpl_bitset.h"

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

bool mpl_bitset_set(const long i, mpl_bitset* b)
{
    b->data[i / NULONGBITS] |= 1UL << (i % NULONGBITS);
    
    return true;
}

bool mpl_bitset_clearbit(long i, mpl_bitset* b)
{
    b->data[i / NULONGBITS] &= ~(1UL << (i % NULONGBITS));
    
    return true;
}

bool mpl_bitset_clearall(mpl_bitset* b)
{
    memset(b->data, 0, b->nfields * sizeof(unsigned long));
    return true;
}

inline bool mpl_bitset_AND
(mpl_bitset* dest, const mpl_bitset* b1, const mpl_bitset* b2)
{
    int i = 0;
    bool ret = false;
    
    for (i = 0; i < b1->nfields; ++i) {
        if ((dest->data[i] = b1->data[i] & b2->data[i])) {
            ret = true;
        }
    }
    
    return ret;
}

void mpl_bitset_OR
(mpl_bitset* dest, const mpl_bitset* b1, const mpl_bitset* b2)
{
    int i = 0;
     
    for (i = 0; i < b1->nfields; ++i) {
        dest->data[i] = b1->data[i] & b2->data[i];
    }
}

inline bool mpl_bitset_XOR
(mpl_bitset* dest, const mpl_bitset* b1, const mpl_bitset* b2)
{
    int i = 0;
    bool ret = false;
    
    for (i = 0; i < b1->nfields; ++i) {
        if ((dest->data[i] = b1->data[i] & b2->data[i])) {
            ret = true;
        }
    }
    
    return ret;
}

void mpl_bitset_flip(mpl_bitset* b)
{
    int i = 0;

    for (i = 0; i < b->nfields; ++i) {
        b->data[i] = ~b->data[i];
    }
}

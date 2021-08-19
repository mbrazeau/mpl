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

mpl_bitset *mpl_bitset_init(const int minbits, mpl_bitset *b)
{
    b->nfields = minbits / NULONGBITS;
    
    if (minbits % NULONGBITS != 0) {
        b->nfields += 1;
    }
    
    b->data = NULL;
    b->data = (unsigned long*)safe_calloc(b->nfields, sizeof(unsigned long));
    
    b->maxbit = minbits;//nb->nfields * NULONGBITS;
    
    return b;
}

mpl_bitset* mpl_bitset_new(const int minbits)
{
    mpl_bitset* nb = NULL;
    nb = (mpl_bitset*)safe_calloc(1, sizeof(mpl_bitset));
    
    if (nb == NULL) {
        return NULL;
    }
  
    if (!mpl_bitset_init(minbits, nb)) {
        safe_free(nb);
        nb = NULL; // Precautionary.
    }

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
        dest->data[i] = b1->data[i] | b2->data[i];
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

int mpl_bitset_cmp(const mpl_bitset* b1, const mpl_bitset* b2)
{
#ifdef DEBUG
    assert(b1->nfields == b2->nfields);
#endif
    return memcmp(b1->data, b2->data, b1->nfields * sizeof(unsigned long));
}

bool mpl_bitset_compat(const mpl_bitset* bits, const mpl_bitset* constr)
{
    int i = 0;
    
    for (i = 0; i < bits->nfields; ++i) {
        if ((bits->data[i] & constr->data[i]) != bits->data[i]) {
            return false;
        }
    }
    
    return true;
}

void mpl_bitset_flip(mpl_bitset* b)
{
    int i = 0;

    for (i = 0; i < b->nfields; ++i) {
        b->data[i] = ~b->data[i];
    }
}

void mpl_bitset2str(mpl_str* str, mpl_bitset* b)
{
    mpl_str_clear(str);
    
    int i = 0;
    for (i = b->nfields-1; i >= 0; --i) {
        int j = 0;
        unsigned long t = (1UL << (NULONGBITS-1));
        if (i == b->nfields-1) {
            t = (1UL << ((b->maxbit % NULONGBITS) - 1));
        }

        unsigned long lim = NULONGBITS;
        if (i == b->nfields-1) {
            lim = b->maxbit % NULONGBITS;
        }
        
        for (j = 0; j < lim; ++j) {
            
            if (b->data[i] & (t >> j)) {
                mpl_str_append('*', str);
            } else {
                mpl_str_append('.', str);
            }
        }
    }
}

//
//  mpl_bitset.h
//  Morphy
//
//  Created by Martin Brazeau on 31/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_bitset_h
#define mpl_bitset_h

#include <limits.h>
#include <stdbool.h>

typedef void* mpl_str;

typedef struct _bitset {
    
    int             nfields;
    int             maxbit;
    unsigned long*  data;
    
} mpl_bitset;

#define NULONGBITS (CHAR_BIT * sizeof(unsigned long))

mpl_bitset* mpl_bitset_new(const int minbits);
void        mpl_bitset_delete(mpl_bitset** b);
bool mpl_bitset_set(const long i, mpl_bitset* b);
bool        mpl_bitset_clearbit(long i, mpl_bitset* b);
bool        mpl_bitset_clearall(mpl_bitset* b);
inline bool mpl_bitset_AND(mpl_bitset* dest, const mpl_bitset* b1, const mpl_bitset* b2);
void        mpl_bitset_OR(mpl_bitset* dest, const mpl_bitset* b1, const mpl_bitset* b2);
inline bool mpl_bitset_XOR(mpl_bitset* dest, const mpl_bitset* b1, const mpl_bitset* b2);
int         mpl_bitset_cmp(const mpl_bitset* b1, const mpl_bitset* b2);
bool        mpl_bitset_compat(const mpl_bitset* bits, const mpl_bitset* constr);
void        mpl_bitset_flip(mpl_bitset* b);
void        mpl_bitset2str(mpl_str* str, mpl_bitset* b);

#endif /* mpl_bitset_h */

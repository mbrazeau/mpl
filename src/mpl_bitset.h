//
//  mpl_bitset.h
//  Morphy
//
//  Created by Martin Brazeau on 31/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef mpl_bitset_h
#define mpl_bitset_h

#include <stdbool.h>

typedef struct _bitset mpl_bitset;

#define NULONGBITS (CHAR_BIT * sizeof(mpl_bitset))

mpl_bitset* mpl_bitset_new(const int minbits);
void        mpl_bitset_delete(mpl_bitset** b);
inline bool mpl_bitset_set(const long i, mpl_bitset* b);
bool        mpl_bitset_clearbit(long i, mpl_bitset* b);
bool        mpl_bitset_clearall(mpl_bitset* b);
inline bool mpl_bitset_AND(mpl_bitset* dest, const mpl_bitset* b1, const mpl_bitset* b2);
inline void mpl_bitset_OR(mpl_bitset* dest, const mpl_bitset* b1, const mpl_bitset* b2);
inline bool mpl_bitset_XOR(mpl_bitset* dest, const mpl_bitset* b1, const mpl_bitset* b2);
void        mpl_bitset_flip(mpl_bitset* b);

#endif /* mpl_bitset_h */

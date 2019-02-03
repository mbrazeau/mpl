//
//  mpl_bitset.c
//  Morphy
//
//  Created by Martin Brazeau on 31/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#include "mpl_bitset.h"

inline bool mpl_bitset_set(const long i, mpl_bitset* b)
{
    b[i / NULONGBITS] |= ( ((mpl_bitset)1) << (i % NULONGBITS) );
    
    return true;
}

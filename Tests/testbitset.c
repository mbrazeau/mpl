//
//  testbitset.c
//  morphy_nui_v03
//
//  Created by Martin Brazeau on 24/03/2020.
//
#include "mpltest.h"

#include "testbitset.h"
#include "mpl_bitset.h"


void print_bits(mpl_bitset* bits)
{
    int i = 0;
    for (i = bits->nfields-1; i >= 0; --i) {
        int j = 0;
        unsigned long b = (1UL << (NULONGBITS-1));
        for (j = 0; j < NULONGBITS; ++j) {
            
            if (bits->data[i] & (b >> j)) {
                printf("%i", 1);
            } else {
                printf("%i", 0);
            }
        }
    }
}

int test_mpl_bitset_basic (void)
{
    theader("Test basic test of bitset.");
    
    int failn = 0;
    
    mpl_bitset* bits = NULL;
    bits = mpl_bitset_new(100);
    
    mpl_bitset_set(2, bits);
    mpl_bitset_set(80, bits);
    
    print_bits(bits);
    printf("\n");
    
    if (bits->data[0] != 0x4) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    if (bits->data[1] != (0x1 << 16)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    return failn;
}

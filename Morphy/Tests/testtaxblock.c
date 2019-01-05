//
//  testtaxblock.c
//  Morphy
//
//  Created by Martin Brazeau on 03/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//
#include <stdio.h>
#include "mpltest.h"
#include "../src/Taxa/mpl_taxblock.h"
#include "testio.h"
#include "testtaxblock.h"

int test_taxa_block_basic (int argc, const char * argv[])
{
    theader("Test basic taxa block allocation and storage");
    
    int failn = 0;
    long ntax = 0;
    
    // TODO: Find a better way to deal with this, or else the program could
    // leak memory like a seive.
    
    char labelbuf[200];
    
    mplNexusReader rdr;
    
    int argnum = 1; // Specify matrix you want to open
    
    rdr = mpl_test_file_open(argnum, argv);
    
    ntax = (long)rdr_get_ntax(rdr);
    
    mpl_taxablock* tb = mpl_taxablock_new(ntax);
    
    int i = 0;
    for (i = 0; i < ntax; ++i) {
        rdr_get_taxon(labelbuf, i, rdr);
        mpl_taxablock_add_taxon(labelbuf, tb);
        memset(labelbuf, 0, 200 * sizeof(char));
    }
    
    printf("\n");
    for (i = 0; i < tb->num_taxa; ++i) {
        printf("%s\n", tb->taxa[i].name);
    }
    
    mpl_taxablock_delete(&tb);
    
    return failn;
}

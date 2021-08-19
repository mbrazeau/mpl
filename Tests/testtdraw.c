//
//  testtdraw.c
//  Morphy
//
//  Created by Martin Brazeau on 17/03/2020.
//  Copyright © 2020 Martin Brazeau. All rights reserved.
//

#include "mpltest.h"

#include "testtdraw.h"
#include "mpl_tdraw.h"
#include "mpl_taxblock.h"
#include "mpl_newick_rdr.h"

int test_new_tree_drawing (void)
{
    theader("Test allocating and setup of a new tree drawing");
    
    int failn = 0;
    
    int ntax = 10;
    
    char* taxa[] = {
        "Alpha",
        "Beta",
        "Gamma",
        "Delta",
        "Epsilon",
        "Eta",
        "Theta",
        "Omega",
        "Rho",
        "Zeta"
    };
    
    char* nwkstring = "((((1,((2,7),(5,9))),(4,8)),6),(3,10));";
    
    mpl_taxablock *tb = mpl_taxablock_new(ntax);
    
    int i = 0;
    for (i = 0; i < ntax; ++i) {
        mpl_taxablock_add_taxon(taxa[i], tb);
    }
    
    mpl_tdraw* tdrw = mpl_tdraw_new(tb);
    
    if (tdrw == NULL) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    printf("\n");

    mpl_tree* t = mpl_new_tree(ntax);
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    mpl_topol_init(ntax, &top);
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    mpl_newick_read(nwkstring, &top, &rdr);
    
    mpl_tree_read_topol(t, &top);
    
    mpl_tdraw_do(t, tdrw);
    
    for (i = 0; i < tdrw->height; ++i) {
        printf("%s", tdrw->canvas[i]);
    }
    
    mpl_tdraw_delete(&tdrw);
    
    return failn;
}

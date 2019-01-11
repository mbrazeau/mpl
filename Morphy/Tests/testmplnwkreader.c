//
//  testmplnwkreader.c
//  Morphy
//
//  Created by Martin Brazeau on 31/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include <string.h>

#include "mpltest.h"
#include "testmplnwkreader.h"
#include "../src/Trees/mpl_tree.h"
#include "../src/Trees/mpl_topol.h"
#include "../src/Trees/mpl_newick_rdr.h"


int test_newick_reader (void)
{
    theader("Test Newick reader");
    
    int failn = 0;
    long numtaxa = 5;
    char* nwkstring = "((1,5),((2,4),3));";
    
    mpl_newick_rdr nwkrdr;
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    mpl_topol_reset(numtaxa, &top);
    
    mpl_newick_rdr_init(numtaxa, &nwkrdr);
    
    mpl_newick_read(nwkstring, &top, &nwkrdr);
    
//    printf("\n\nVerify reader tree: \n");
//    mpl_node_bin_traverse_temp(nwkrdr.tree->base);
//    printf("\n\n");
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    
    mpl_tree_read_topol(t, &top);
    
    printf("Target: %s\n", nwkstring);
    char* nwkop = NULL;
    mpl_tree_write_newick(&nwkop, t);
    printf("Result: %s", nwkop);
    printf("\n");
    
    if (strcmp(nwkop, nwkstring)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    free(nwkop);
    
    free(nwkrdr.namebuffer);
    
    return failn;
}

int test_newick_reader_bigger_tree (void)
{
    theader("Test Newick reader on larger tree");
    
    int failn = 0;
    int err = 0;
    
    long numtaxa = 10;
    char* nwkstring = "((((1,((2,7),(5,9))),(4,8)),6),(3,10));";
    
    mpl_newick_rdr nwkrdr;
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    mpl_topol_reset(numtaxa, &top);
    
    mpl_newick_rdr_init(numtaxa, &nwkrdr);
    
    err = mpl_newick_read(nwkstring, &top, &nwkrdr);
    //mpl_node_bin_traverse_temp(nwkrdr.tree->base);

    if (err) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    
    
    mpl_tree_read_topol(t, &top);
    
    printf("Target: %s\n", nwkstring);
    char* nwkop = NULL;
    mpl_tree_write_newick(&nwkop, t);
    printf("Result: %s", nwkop);
    printf("\n");
    
    if (strcmp(nwkop, nwkstring)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    free(nwkop);
    
    printf("\n");
    free(nwkrdr.namebuffer);
    
    return failn;
}

int test_newick_mult_large_newick_reads (void)
{
    theader("Test multiple large Newick reads");

    int failn = 0;
    int err = 0;

    long numtaxa = 12;
    long nstr = 3;
    char* nwkstrs[] = {"((((((1,2),3),4),5),6),(7,(8,(9,(10,(11,12))))));",
                        "(1,(2,(3,(4,(5,(6,(7,(8,(9,(10,(11,12)))))))))));",
                        "((((1,((2,(7,11)),(5,9))),((12,4),8)),6),(3,10));"
    };

    mpl_newick_rdr nwkrdr;
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
//    mpl_topol_reset(numtaxa, &top);

    mpl_newick_rdr_init(numtaxa, &nwkrdr);
    
    mpl_tree* t = mpl_new_tree(numtaxa);

    int i = 0;
    for (i = 0; i < nstr; ++i) {
        char* nwkop = NULL;
        mpl_topol_reset(numtaxa, &top);
        err = mpl_newick_read(nwkstrs[i], &top, &nwkrdr);
        
        if (err) {
            ++failn;
            err = 0;
            pfail;
        }
        else {
            ppass;
        }
        
        mpl_tree_read_topol(t, &top);
        
        printf("Target: %s\n", nwkstrs[i]);
        mpl_tree_write_newick(&nwkop, t);
        printf("Result: %s", nwkop);
        printf("\n");
        
        if (strcmp(nwkop, nwkstrs[i])) {
            ++failn;
            pfail;
        }
        else {
            ppass;
        }
        
        free(nwkop);
        mpl_tree_reset(t);
    }
    
    free(nwkrdr.namebuffer);
    mpl_delete_tree(&t);
    return failn;
}

int test_newick_bulk_reads (void)
{
    theader("Test bulk reading newicks");
    
    int failn = 0;
    int err = 0;
    
    long numtaxa = 10;
    long nstr = 12;
    char* nwkstrs[] = {
        "((((1,((2,7),(5,9))),(4,8)),6),(3,10));",
        "((1,3),(((((((2,10),6),8),9),7),5),4));",
        "(((1,(((2,4),3),((7,9),8))),(5,10)),6);",
        "(((((((1,8),(3,4)),(2,6)),9),7),5),10);",
        "((((((1,(4,6)),10),5),9),((2,8),7)),3);",
        "((((8,4),(((5,9),(2,7)),1)),6),(10,3));", // Same as 0
        "((((((((1,2),3),(8,10)),9),7),4),6),5);",
        "(((((1,5),(((2,10),3),4)),9),6),(7,8));",
        "((((((1,6),(3,(4,8))),7),10),(2,5)),9);",
        "(((((1,2),10),(3,(5,((7,8),9)))),4),6);",
        "(1,(((2,(((3,6),((5,10),7)),4)),9),8));",
        "((((((1,10),5),((4,6),7)),2),9),(3,8));"
    };
    
    mpl_newick_rdr nwkrdr;
    mpl_topol top;
    top.num_taxa = 1;
    top.edges = NULL;
    //    mpl_topol_reset(numtaxa, &top);
    
    mpl_newick_rdr_init(numtaxa, &nwkrdr);
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    
    int i = 0;
    for (i = 0; i < nstr; ++i) {
        char* nwkop = NULL;
        mpl_topol_reset(numtaxa, &top);
        err = mpl_newick_read(nwkstrs[i], &top, &nwkrdr);
        
        if (err) {
            ++failn;
            err = 0;
            pfail;
        }
        else {
            ppass;
        }
        
        mpl_tree_read_topol(t, &top);
        
        printf("Target: %s\n", nwkstrs[i]);
        mpl_tree_write_newick(&nwkop, t);
        printf("Result: %s", nwkop);
        printf("\n");
        
        if (strcmp(nwkop, nwkstrs[i])) {
            ++failn;
            pfail;
        }
        else {
            ppass;
        }
        
        free(nwkop);
        mpl_tree_reset(t);
    }
    
    free(nwkrdr.namebuffer);
    mpl_delete_tree(&t);
    return failn;
}

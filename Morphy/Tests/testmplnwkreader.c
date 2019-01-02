//
//  testmplnwkreader.c
//  Morphy
//
//  Created by Martin Brazeau on 31/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include "mpltest.h"
#include "testmplnwkreader.h"
#include "../src/Trees/mpl_tree.h"
#include "../src/Trees/mpl_topol.h"
#include "../src/Trees/mpl_newick_rdr.h"

static void mpl_node_bin_traverse_temp(mpl_node* n)
{
    if (n->tip) {
        printf("%li", n->tip);
        return;
    }
    
    printf("(");
    mpl_node_bin_traverse_temp(n->left);
    printf(",");
    mpl_node_bin_traverse_temp(n->right);
    printf(")");
}

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
    
    printf("\n");
    
    mpl_node_bin_traverse_temp(t->base);
    
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
    printf("Result: ");
    mpl_node_bin_traverse_temp(t->base);
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
    for (i = 2; i < nstr; ++i) {
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
        printf("Result: ");
        mpl_node_bin_traverse_temp(t->base);
        printf("\n");

    }
    
    free(nwkrdr.namebuffer);
    mpl_delete_tree(&t);
    return failn;
}


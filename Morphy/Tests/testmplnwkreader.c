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
    char* nwkstring = "((1,5),(3,(2,4)));";
    
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
    
    long numtaxa = 6;
    char* nwkstring = "((3,((5,6),4)),(1,2));";
    
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


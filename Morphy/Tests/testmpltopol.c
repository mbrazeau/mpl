//
//  testmpltopol.c
//  Morphy
//
//  Created by Martin Brazeau on 31/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//

#include <string.h>

#include "testmpltopol.h"
#include "mpltest.h"
#include "testmplnode.h"
#include "../src/Trees/mpl_node.h"
#include "../src/Trees/mpl_tree.h"
#include "../src/Trees/mpl_topol.h"
#include "../src/Trees/mpl_newick_rdr.h"
#include "testmpltree.h"

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

int test_rebasing_topology (void)
{
    theader("Test rebasing a tree by rebasing topology");
    
    int failn = 0;
    
    long numtaxa = 5;
    char* nwkstring = "((1,5),((2,4),3));";
    
    mpl_newick_rdr nwkrdr;
    mpl_topol top;
    mpl_topol top2;
    top.num_taxa = 1;
    top2.num_taxa = 1;
    top.edges = NULL;
    top2.edges = NULL;
    mpl_topol_reset(numtaxa, &top);
    mpl_topol_reset(numtaxa, &top2);
    
    mpl_newick_rdr_init(numtaxa, &nwkrdr);
    
    mpl_newick_read(nwkstring, &top, &nwkrdr);
    
    //    printf("\n\nVerify reader tree: \n");
    //    mpl_node_bin_traverse_temp(nwkrdr.tree->base);
    //    printf("\n\n");
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    t->num_polys = 20;
    
    mpl_tree_read_topol(t, &top);
    
    mpl_record_topol(&top2, t);
    
    mpl_topol_rebase(0, &top2);

    mpl_tree_reset(t);
    mpl_tree_read_topol(t, &top2);

    mpl_node_bin_traverse_temp(t->base);

    mpl_delete_tree(&t);
    
    return failn;
}

int test_rebasing_large_topology (void)
{
    theader("Test rebasing a tree by rebasing topology");
    
    int failn = 0;
    
    long numtaxa = 10;
    char* nwkstring = "((((1,((2,7),(5,9))),(4,8)),6),(3,10));";
    
    mpl_newick_rdr nwkrdr;
    mpl_topol top;
    mpl_topol top2;
    top.num_taxa = 1;
    top2.num_taxa = 1;
    top.edges = NULL;
    top2.edges = NULL;
    mpl_topol_reset(numtaxa, &top);
    mpl_topol_reset(numtaxa, &top2);
    
    mpl_newick_rdr_init(numtaxa, &nwkrdr);
    
    mpl_newick_read(nwkstring, &top, &nwkrdr);
    
    //    printf("\n\nVerify reader tree: \n");
    //    mpl_node_bin_traverse_temp(nwkrdr.tree->base);
    //    printf("\n\n");
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    t->num_polys = 20;
    
    mpl_tree_read_topol(t, &top);
    
    mpl_record_topol(&top2, t);
    
    mpl_topol_rebase(7, &top2);
    
    mpl_tree_reset(t);
    mpl_tree_read_topol(t, &top2);
    
    char* nwkop = NULL;
    char* testnwk = NULL;
    mpl_tree_write_newick(&nwkop, t);
    printf("Result 1: %s", nwkop);
    printf("\n");
    
    // Read this tree back as a topology
    mpl_topol_reset(numtaxa, &top);
//    mpl_newick_rdr_init(numtaxa, &nwkrdr);
    mpl_newick_read(nwkop, &top, &nwkrdr);
    
    mpl_tree_reset(t);
    mpl_tree_read_topol(t, &top);
    mpl_tree_write_newick(&testnwk, t);
    
    printf("Result 2: %s", testnwk);
    printf("\n");
    
    if (strcmp(nwkop, testnwk)) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    free(nwkop);
    free(testnwk);
    
    mpl_delete_tree(&t);
    
    return failn;
}

int test_topology_comparison (void)
{
    theader("Test simple topology comparison");
    
    int failn = 0;
    
    int ntax = 5;
    char* nwk1 = "((1,5),((2,4),3));";
    char* nwk2 = "((5,1),(3,(2,4)));";
    
    mpl_topol* tp1 = mpl_topol_new(ntax);
    mpl_topol* tp2 = mpl_topol_new(ntax);
    
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    
    mpl_newick_read(nwk1, tp1, &rdr);
    mpl_newick_read(nwk2, tp2, &rdr);
    
    // Perform a simple check based on direct inputs
    
    int res = 0;
    
    res = mpl_topol_compare(tp1, tp2);
    
    if (res) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    // Now build a tree from each and compare the outputs.
    
    mpl_tree* t = NULL;
    t = mpl_new_tree(ntax);
    
    mpl_topol* tp3 = mpl_topol_new(ntax);
    mpl_topol* tp4 = mpl_topol_new(ntax);
    
    mpl_tree_read_topol(t, tp1);
    mpl_record_topol(tp3, t);
    
    mpl_tree_read_topol(t, tp2);
    mpl_record_topol(tp4, t);
    
    
    res = 0;
    res = mpl_topol_compare(tp3, tp4);
    
    if (res) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_delete_tree(&t);
    mpl_topol_delete(&tp1);
    mpl_topol_delete(&tp2);
    mpl_topol_delete(&tp3);
    mpl_topol_delete(&tp4);
    
    if (rdr.namebuffer) {
        free(rdr.namebuffer);
        rdr.namebuffer = NULL;
    }
    
    return failn;
}

int test_topology_comparison_after_rebase (void)
{
    theader("Test simple topology comparison");
    
    int failn = 0;
    
    int ntax = 5;
    char* nwk1 = "((1,5),((2,4),3));";
    char* nwk2 = "((5,1),(3,(2,4)));";
    
    mpl_topol* tp1 = mpl_topol_new(ntax);
    mpl_topol* tp2 = mpl_topol_new(ntax);
    
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    
    mpl_newick_read(nwk1, tp1, &rdr);
    mpl_newick_read(nwk2, tp2, &rdr);
    
    // Rebase the topologies:
    mpl_topol_rebase(0, tp1);
    mpl_topol_rebase(0, tp2);
    
    // Perform a simple check based on direct inputs
    
    int res = 0;
    
    res = mpl_topol_compare(tp1, tp2);
    
    if (res) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    // Now build a tree from each and compare the outputs.
    
    mpl_tree* t = NULL;
    t = mpl_new_tree(ntax);
    
    mpl_topol* tp3 = mpl_topol_new(ntax);
    mpl_topol* tp4 = mpl_topol_new(ntax);
    
    mpl_tree_read_topol(t, tp1);
    mpl_record_topol(tp3, t);
    
    mpl_tree_read_topol(t, tp2);
    mpl_record_topol(tp4, t);
    
    mpl_topol_rebase(0, tp3);
    mpl_topol_rebase(0, tp4);
    
    res = 0;
    res = mpl_topol_compare(tp3, tp4);
    
    if (res) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_delete_tree(&t);
    mpl_topol_delete(&tp1);
    mpl_topol_delete(&tp2);
    mpl_topol_delete(&tp3);
    mpl_topol_delete(&tp4);
    
    if (rdr.namebuffer) {
        free(rdr.namebuffer);
        rdr.namebuffer = NULL;
    }
    
    return failn;
}

int test_negative_topology_comparison (void)
{
    theader("Test simple topology comparison");
    
    int failn = 0;
    
    int ntax = 5;
    char* nwk1 = "((1,3),((2,4),5));";
    char* nwk2 = "((5,1),(3,(2,4)));";
    
    mpl_topol* tp1 = mpl_topol_new(ntax);
    mpl_topol* tp2 = mpl_topol_new(ntax);
    
    mpl_newick_rdr rdr;
    mpl_newick_rdr_init(ntax, &rdr);
    
    mpl_newick_read(nwk1, tp1, &rdr);
    mpl_newick_read(nwk2, tp2, &rdr);
    
    // Perform a simple check based on direct inputs
    
    int res = 0;
    
    res = mpl_topol_compare(tp1, tp2);
    
    if (!res) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    // Now build a tree from each and compare the outputs.
    
    mpl_tree* t = NULL;
    t = mpl_new_tree(ntax);
    
    mpl_topol* tp3 = mpl_topol_new(ntax);
    mpl_topol* tp4 = mpl_topol_new(ntax);
    
    mpl_tree_read_topol(t, tp1);
    mpl_record_topol(tp3, t);
    
    mpl_tree_read_topol(t, tp2);
    mpl_record_topol(tp4, t);
    
    
    res = 0;
    res = mpl_topol_compare(tp3, tp4);
    
    if (!res) {
        ++failn;
        pfail;
    }
    else {
        ppass;
    }
    
    mpl_delete_tree(&t);
    mpl_topol_delete(&tp1);
    mpl_topol_delete(&tp2);
    mpl_topol_delete(&tp3);
    mpl_topol_delete(&tp4);
    
    if (rdr.namebuffer) {
        free(rdr.namebuffer);
        rdr.namebuffer = NULL;
    }
    
    return failn;
}

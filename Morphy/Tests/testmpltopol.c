//
//  testmpltopol.c
//  Morphy
//
//  Created by Martin Brazeau on 31/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//


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
    
    mpl_node_bin_traverse_temp(t->base);
    
    mpl_delete_tree(&t);
    
    return failn;
}

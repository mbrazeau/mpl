//
//  testmpltree.c
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//
#include "mpltest.h"
#include "testmplnode.h"
#include "../src/Trees/mpl_node.h"
#include "../src/Trees/mpl_tree.h"
#include "../src/Trees/mpl_topol.h"
#include "testmpltree.h"

// TODO: Remove/replace:
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

int test_tree_assembly_from_topology (void)
{
    theader("Test assembling tree from topology");
    
    int failn = 0;
    
    int numtaxa = 5;
    int numnodes = 2 * numtaxa - 1;
//    long edgetable[] = {5, 6, 7, 6, 5, 8, 7, 8, -1};
//    long edgetable[] = {6, 8, 7, 8, 6, -1, 5, 5, 7 };
    long edgetable[] =   {6, 8, 7, 8, 6, -1, 5, 5, 7};
//    long edgetable[] = {7, 5, 6, 5, 7, 6, 8, 8, -1 };
    
    mpl_topol top;
    top.num_taxa = numtaxa;
    top.num_nodes = numnodes;
    top.edges = &edgetable[0];
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    mpl_tree_read_topol(t, &top);
    
    mpl_node_bin_traverse_temp(t->base);
    printf("\n");
    
    // TODO: Finish
    
    mpl_delete_tree(&t);
    
    return failn;
}

int test_binary_postorder (void)
{
    theader("Test binary postorder");
    
    int failn = 0;
    
    int numtaxa = 5;
    int numnodes = 2 * numtaxa - 1;
    long edgetable[] = {5, 6, 7, 6, 5, 8, 7, 8, -1};
    long exp_order[] = {0, 4, 5, 2, 1, 3, 6, 7, 8};
    
    mpl_topol top;
    top.num_taxa = numtaxa;
    top.num_nodes = numnodes;
    top.edges = &edgetable[0];
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    mpl_tree_read_topol(t, &top);
    
    mpl_node_bin_traverse_temp(t->base);
    printf("\n");
    
    mpl_tree_traverse(t);
    
    int i = 0;
    for (i = 0; i < numnodes-1; ++i) {
        if (t->postord_all[i]->mem_index != exp_order[i]) {
            ++failn;
            pfail;
        }
        else {
            ppass;
        }
    }
    
    mpl_delete_tree(&t);
    
    return failn;
}
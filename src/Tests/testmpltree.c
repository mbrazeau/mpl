//
//  testmpltree.c
//  Morphy
//
//  Created by Martin Brazeau on 29/12/2018.
//  Copyright © 2018 Martin Brazeau. All rights reserved.
//
#include "mpltest.h"
#include "testmplnode.h"
#include "../Trees/mpl_node.h"
#include "../Trees/mpl_tree.h"
#include "../Trees/mpl_topol.h"
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
    long edgetable[] = {5, 6, 7, 6, 5, 8, 7, 8, -1};
    
    mpl_topol top;
    top.num_taxa = numtaxa;
    top.num_nodes = numnodes;
    top.edges = &edgetable[0];
    
    mpl_tree* t = mpl_new_tree(numtaxa);
    mpl_tree_read_topol(t, &top);
    
    mpl_node_bin_traverse_temp(t->base);
    
    return failn;
}

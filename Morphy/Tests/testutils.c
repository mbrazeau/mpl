//
//  testutils.c
//  Morphy
//
//  Created by Martin Brazeau on 05/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//
#include <stdio.h>

#include "../src/Trees/mpl_tree.h"

void mpl_test_print_bin_traverse(mpl_node* n);

void mpl_test_print_bin_traverse(mpl_node* n)
{
    if (n->tip) {
        printf("%li", n->tip);
        return;
    }
    
    printf("(");
    mpl_test_print_bin_traverse(n->left);
    printf(",");
    mpl_test_print_bin_traverse(n->right);
    printf(")");
}

void mpl_tests_tree_traverse(mpl_tree* t)
{
    mpl_test_print_bin_traverse(t->base);
}

int mpl_tree_checker(const mpl_tree* t)
{
    long i = 0;
    int err = 0;
    
    mpl_node* n = NULL;
    mpl_node** p = NULL;
    
    for (i = t->num_taxa; i < t->num_nodes; ++t) {
        n = NULL;
        
        n = &t->nodes[i];
        
        if (n->descs[0] != n->left) {
            printf("Warning! left pointer not pointing to same node as start of desc array!\n");
        }
        if (n->descs[n->ndescs - 1] != n->right) {
            printf("Warning! Right pointer not pointing to same node as end of desc array!\n");
        }
        
        p = NULL;
        
        p = &n->descs[0];
        
        while (*p) {
            if ((*p)->anc != n) {
                ++err;
                printf ("Your goddamned tree is broken!\n");
            }
            ++p;
        }
    }
    
    return err;
}

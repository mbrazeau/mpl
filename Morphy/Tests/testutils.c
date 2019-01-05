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

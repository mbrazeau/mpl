//
//  testutils.h
//  Morphy
//
//  Created by Martin Brazeau on 05/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef testutils_h
#define testutils_h
#include "mpl_tree.h"
#include "mpl_matrix.h"

void mpl_tests_tree_traverse(mpl_tree* t);
int mpl_tree_checker(const mpl_tree* t);
void mpl_test_print_newick(mpl_node* n);
void mpl_test_print_tree(mpl_tree* t);
void mpl_test_print_charchanges(mpl_matrix* m);
void add_local_changes(mpl_charbuf* cb);

#endif /* testutils_h */

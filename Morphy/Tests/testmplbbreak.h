//
//  testmplbbreak.h
//  Morphy
//
//  Created by Martin Brazeau on 15/01/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef testmplbbreak_h
#define testmplbbreak_h

#include "../src/Analysis/mpl_bbreak.h"

// Unit test functions:
int test_bbreak_reroot_all_for_subtree (void);
int test_bbreak_store_allswaps (void);
int test_bbreak_store_TBR_allswaps (void);
// Test interface functions:
int mpl_test_bbreak_tbr_reroot(mpl_node* tgtnbr, mpl_node* base);
void mpl_test_branch_swap(mpl_tree* t, mpl_bbreak* bbk);

#endif /* testmplbbreak_h */

//
//  testmplstepwise.h
//  Morphy
//
//  Created by Martin Brazeau on 02/02/2019.
//  Copyright © 2019 Martin Brazeau. All rights reserved.
//

#ifndef testmplstepwise_h
#define testmplstepwise_h

#include "../src/mpl_defs.h"
#include "../src/Analysis/mpl_stepwise.h"

int test_initial_fork (void);
int test_build_arbitrary_tree (void);
int test_build_med_tree (void);
int test_build_large_tree (void);

void mpl_test_setup_first_fork(mpl_stepwise* sw);

#endif /* testmplstepwise_h */
